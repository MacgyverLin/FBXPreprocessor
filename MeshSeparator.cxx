#include "MeshSeparator.h"
#include "Polygon.h"
#include "BSP.h"

MeshSeparator::MeshSeparator()
{
}

MeshSeparator::~MeshSeparator()
{
}

bool MeshSeparator::Process(const std::vector<Mesh>& srcMeshes_, std::vector<MeshArray>& resultMeshArrays_)
{
	resultMeshArrays_.resize(srcMeshes_.size());
	for (size_t i = 0; i < srcMeshes_.size(); i++)
	{
		resultMeshArrays_[i].resize(1);

		if (!Process(srcMeshes_[i], resultMeshArrays_[i][0]))
			return false;
	}

	return true;
}

bool MeshSeparator::Process(const Mesh& srcMesh_, Mesh& resultMesh_)
{
	BSP bsp;
	bsp.splitPlane = Plane(
		//srcMesh_.GetAABB().GetMajorAxis(true),
		//srcMesh_.GetAABB().GetCenter(true)
		Vector3::UnitY,
		srcMesh_.GetAABB().GetCenter(false)
	);

	// make sure src Mesh is closed
	if (!srcMesh_.IsClosed())
		return false;

	// slice, use bsp to set groupID
	if (!Slice(srcMesh_, &bsp, resultMesh_))
		return false;

	//std::vector<Loop> crossSectionLoops;
	//if (!BuildCrossSectionLoops(resultMesh_, crossSectionLoops))
		//return false;

	//if (!PrintCrossSectionLoops(resultMesh_, crossSectionLoops))
		//return false;

	std::vector<Loop2> crossSectionLoops2;
	if (!BuildCrossSectionLoops2(resultMesh_, crossSectionLoops2))
		return false;

	if (!PrintCrossSectionLoops2(resultMesh_, crossSectionLoops2))
		return false;

	if (!AddCrossSectionLoopsToMesh2(crossSectionLoops2, resultMesh_))
		return false;

	return true;
}

bool MeshSeparator::Slice(const Mesh& srcMesh_, BSP* root_, Mesh& resultMesh_)
{
	int uvChannelCount = srcMesh_.GetUVChannelCount();
	resultMesh_.Begin(srcMesh_.GetColorChannelCount(), uvChannelCount + 1, srcMesh_.GetNormalChannelCount(), srcMesh_.GetTangentChannelCount(), srcMesh_.GetBinormalChannelCount());

	int groupIDUVChannel = resultMesh_.GetUVChannelCount() - 1;
	for (size_t polyIdx = 0; polyIdx < srcMesh_.GetPolygonCount(); polyIdx++)
	{
		const Vector3& center = srcMesh_.GetPolygonCenter(polyIdx);
		int groupID = GetGroupIdx(srcMesh_, root_, center);

		resultMesh_.BeginPolygon(groupID, srcMesh_.GetPolygonMaterialIdx(polyIdx));
		for (size_t edgeIdx = 0; edgeIdx < srcMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			Vertex v = srcMesh_.GetPolygonVertex(polyIdx, edgeIdx);
			v.uvs[groupIDUVChannel].X() = groupID;
			v.uvs[groupIDUVChannel].Y() = 0; // not crossection

			resultMesh_.AddPolygonVertex(v);
		}
		resultMesh_.EndPolygon();
	}

	resultMesh_.End();

	return true;
}

int MeshSeparator::GetGroupIdx(const Mesh& srcMesh_, BSP* root_, const Vector3& p_)
{
	int polygonGroupID = 0;
	if (root_->GetPlane().WhichSide(p_))
	{
		if(p_.Z() > 0.5f)
			polygonGroupID = 0;
		else
			polygonGroupID = 1;
	}
	else
	{
		polygonGroupID = 2;
	//	if (p_.Z() > 0.5f)
		//	polygonGroupID = 2;
		//else
			//polygonGroupID = 3;
	}

	return polygonGroupID;
}

bool MeshSeparator::BuildCrossSectionLoops2(const Mesh& resultMesh_, std::vector<Loop2>& crossSectionLoops_)
{
	std::map<int, UnorderedEdges> unorderedEdgesGroups;
	if (!BuildCrossSectionEdges2(resultMesh_, unorderedEdgesGroups))
		return false;

	if (!BuildCrossSectionLoopsFromEdges2(unorderedEdgesGroups, crossSectionLoops_))
		return false;

	return true;
}

bool MeshSeparator::BuildCrossSectionEdges2(const Mesh& resultMesh_, std::map<int, UnorderedEdges>& unorderedEdgesGroups_)
{
	for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
	{
		for (size_t edgeIdx = 0; edgeIdx < resultMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			int groupID = resultMesh_.GetPolygonGroupID(polyIdx);


			int polygonEdgeAdjacentPolygonIdx = resultMesh_.GetPolygonEdgeAdjacentPolygonIdx(polyIdx, edgeIdx);
			int adjacentGroupID = resultMesh_.GetPolygonGroupID(polygonEdgeAdjacentPolygonIdx);

			if (groupID != adjacentGroupID)
			{
				const Edge& edge = resultMesh_.GetPolygonEdge(polyIdx, edgeIdx);
				auto& unorderedEdges = unorderedEdgesGroups_[groupID];
				// if (unorderedEdges.find(edge.GetStartIdx()) != unorderedEdges.end())
					// return false;

				unorderedEdges.insert
				(
					std::pair<int, std::pair<int, bool>>
					(
						edge.GetStartIdx(),
						std::pair<int, bool>(edge.GetEndIdx(), false)
						)
				);
			}
		}
	}

	return true;
}

bool MeshSeparator::BuildCrossSectionLoopsFromEdges2(std::map<int, UnorderedEdges> unorderedEdgesGroups_, std::vector<Loop2>& crossSectionLoops_)
{
	auto& FindFirstUnusedEdge = [&](UnorderedEdges& unorderedEdges_)->UnorderedEdges::iterator
	{
		for (UnorderedEdges::iterator& unorderedEdgesItr = unorderedEdges_.begin(); unorderedEdgesItr != unorderedEdges_.end(); unorderedEdgesItr++)
		{
			if (!unorderedEdgesItr->second.second)
				return unorderedEdgesItr;
		}

		return unorderedEdges_.end();
	};


	for (auto& unorderedEdgesGroupsItr = unorderedEdgesGroups_.begin(); unorderedEdgesGroupsItr != unorderedEdgesGroups_.end(); unorderedEdgesGroupsItr++)
	{
		int groupID = unorderedEdgesGroupsItr->first;
		UnorderedEdges& unorderedEdges = unorderedEdgesGroupsItr->second;

		//Debug::Info("Group %d\n", groupID);

		UnorderedEdges::iterator currentEdgeItr = unorderedEdges.begin();
		while (currentEdgeItr != unorderedEdges.end())
		{
			//Debug::Info("Loop %d\n", crossSectionLoops_.size());

			crossSectionLoops_.push_back(Loop2(groupID));
			Loop2& currentLoop = crossSectionLoops_.back();
			do
			{
				currentLoop.push_back(currentEdgeItr->first);
				//Debug::Info("%d\t", currentEdgeItr->first);

				int next = currentEdgeItr->second.first;
				// currentEdgeItr->second.second = true;
				unorderedEdges.erase(currentEdgeItr);

				currentEdgeItr = unorderedEdges.find(next);
				
				// while (currentEdgeItr->second.second && currentEdgeItr!= unorderedEdges.end())
					// currentEdgeItr++;
			} while (currentEdgeItr!= unorderedEdges.end());

			// find adj
			currentEdgeItr = FindFirstUnusedEdge(unorderedEdges);

			//Debug::Info("\n");
		};
	}

	return true;
}

bool MeshSeparator::PrintCrossSectionLoops2(const Mesh& resultMesh_, const std::vector<Loop2>& crossSectionLoops_)
{
	Debug::Info("Triangle Index\t");
	for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
		Debug::Info("%d\t \t \t \t", polyIdx);
	Debug::Info("\n");

	Debug::Info("Center\t");
	for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
	{
		const Vector3& center = resultMesh_.GetPolygonCenter(polyIdx);
		Debug::Info("(%2.3f,%2.3f,%2.3f)\t \t \t \t", center.X(), center.Y(), center.Z());
	}
	Debug::Info("\n");

	Debug::Info("GroupID/Adj Tri.Index\t");
	for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
	{
		int groupID = resultMesh_.GetPolygonGroupID(polyIdx);
		Debug::Info("%d\t", groupID);

		for (size_t edgeIdx = 0; edgeIdx < resultMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			const Edge& edge = resultMesh_.GetPolygonEdge(polyIdx, edgeIdx);
			Debug::Info("%d\t", edge.GetAdjacentPolygonIdx());
		}
	}
	Debug::Info("\n");

	Debug::Info("Edge Start Index\t");
	for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
	{
		int groupID = resultMesh_.GetPolygonGroupID(polyIdx);
		Debug::Info("x\t");

		for (size_t edgeIdx = 0; edgeIdx < resultMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			const Edge& edge = resultMesh_.GetPolygonEdge(polyIdx, edgeIdx);
			Debug::Info("%d\t", edge.GetStartIdx());
		}
	}
	Debug::Info("\n");

	Debug::Info("Edge End Index\t");
	for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
	{
		int groupID = resultMesh_.GetPolygonGroupID(polyIdx);
		Debug::Info("x\t");

		for (size_t edgeIdx = 0; edgeIdx < resultMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			const Edge& edge = resultMesh_.GetPolygonEdge(polyIdx, edgeIdx);
			Debug::Info("%d\t", edge.GetEndIdx());
		}
	}
	Debug::Info("\n");

	for (size_t groupID = 0; groupID < resultMesh_.GetMaxGroupIdx() + 1; groupID++)
	{
		Debug::Info("GroupID %d, Cross Section Edge Start Index\t", groupID);
		for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
		{
			for (size_t edgeIdx = 0; edgeIdx < resultMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
			{
				if (groupID != resultMesh_.GetPolygonGroupID(polyIdx))
					continue;

				int polygonEdgeAdjacentPolygonIdx = resultMesh_.GetPolygonEdgeAdjacentPolygonIdx(polyIdx, edgeIdx);
				int adjacentGroupID = resultMesh_.GetPolygonGroupID(polygonEdgeAdjacentPolygonIdx);
				if (groupID != adjacentGroupID)
				{
					const Edge& edge = resultMesh_.GetPolygonEdge(polyIdx, edgeIdx);
					Debug::Info("%d\t", edge.GetStartIdx());
				}
			}
		}
		Debug::Info("\n");

		Debug::Info("GroupID %d, Cross Section Edge End Index\t", groupID);
		for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
		{
			for (size_t edgeIdx = 0; edgeIdx < resultMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
			{
				if (groupID != resultMesh_.GetPolygonGroupID(polyIdx))
					continue;

				int polygonEdgeAdjacentPolygonIdx = resultMesh_.GetPolygonEdgeAdjacentPolygonIdx(polyIdx, edgeIdx);
				int adjacentGroupID = resultMesh_.GetPolygonGroupID(polygonEdgeAdjacentPolygonIdx);
				if (groupID != adjacentGroupID)
				{
					const Edge& edge = resultMesh_.GetPolygonEdge(polyIdx, edgeIdx);
					Debug::Info("%d\t", edge.GetEndIdx());
				}
			}
		}
		Debug::Info("\n");
	}

	///////////////////////////////////////////////////////////
	int i = 0;
	for (auto& crossSectionLoop : crossSectionLoops_)
	{
		Debug::Verbose("Loop %d, Groupd ID=%d\t", i++, crossSectionLoop.groupID);

		for (int i = 0; i < crossSectionLoop.size(); i++)
		{
			Debug::Verbose("%d\t", crossSectionLoop[i]);;
		}
		Debug::Verbose("\n");
	}

	return true;
}

bool MeshSeparator::AddCrossSectionLoopsToMesh2(const std::vector<Loop2>& crossSectionLoops_, Mesh& resultMesh_)
{
	int groupIDUVChannel = resultMesh_.GetUVChannelCount() - 1;

	resultMesh_.BeginAppend();

	for (auto& crossSectionLoop : crossSectionLoops_)
	{
		resultMesh_.BeginPolygon(crossSectionLoop.groupID, resultMesh_.GetMaxMaterialIdx() + 1);
		// resultMesh_.BeginPolygon(crossSectionLoop.groupID, crossSectionLoop.groupID + 10);

		for (int i = 0; i < crossSectionLoop.size(); i++)
		{
			const int &edgeVertexIdx = crossSectionLoop[crossSectionLoop.size() - 1 - i];

			Vertex vertex;
			vertex.position = resultMesh_.GetEdgeStartVertex(edgeVertexIdx);
			vertex.uvs[0].X() = 0;
			vertex.uvs[0].Y() = 0; // is cross section
			vertex.uvs[groupIDUVChannel].X() = crossSectionLoop.groupID;
			vertex.uvs[groupIDUVChannel].Y() = 1; // is cross section

			resultMesh_.AddPolygonVertex(vertex);
		}
		resultMesh_.EndPolygon();
	}

	resultMesh_.End();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MeshSeparator::BuildCrossSectionLoops(const Mesh& resultMesh_, std::vector<Loop>& crossSectionLoops_)
{
	for (int groupID = 0; groupID <= resultMesh_.GetMaxGroupIdx(); groupID++)
	{
		std::multimap<int, Edge> crossSectionEdges;
		if (!BuildCrossSectionEdges(resultMesh_, groupID, crossSectionEdges))
			return false;

		if (!BuildCrossSectionLoopsFromEdges(crossSectionEdges, groupID, crossSectionLoops_))
			return false;
	}

	return true;
}

bool MeshSeparator::BuildCrossSectionEdges(const Mesh& resultMesh_, int groupID_, std::multimap<int, Edge>& crossSectionEdges_)
{
	// if polygon neighbour ID is not myself, save the edge in the map
	for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
	{
		if (groupID_ != resultMesh_.GetPolygonGroupID(polyIdx))
			continue;

		for (size_t edgeIdx = 0; edgeIdx < resultMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			int polygonEdgeAdjacentPolygonIdx = resultMesh_.GetPolygonEdgeAdjacentPolygonIdx(polyIdx, edgeIdx);

			int groupID = resultMesh_.GetPolygonGroupID(polyIdx);
			int adjacentGroupID = resultMesh_.GetPolygonGroupID(polygonEdgeAdjacentPolygonIdx);

			if (groupID != adjacentGroupID)
			{
				const Edge& edge = resultMesh_.GetPolygonEdge(polyIdx, edgeIdx);
				crossSectionEdges_.insert(std::pair<int, Edge>(edge.GetStartIdx(), edge));
			}
		}
	}

	return true;
}

bool MeshSeparator::BuildCrossSectionLoopsFromEdges(const std::multimap<int, Edge> crossSectionEdges_, int groupID_, std::vector<Loop>& isolatedLoops_)
{
	std::multimap<int, Edge> crossSectionEdges = crossSectionEdges_;

	bool newLoop = true;
	std::multimap<int, Edge>::iterator currentEdge = crossSectionEdges.begin();
	while (crossSectionEdges.size())
	{
		if (newLoop)
		{
			isolatedLoops_.push_back(Loop(groupID_));
			newLoop = false;
		}

		Loop& currentLoop = isolatedLoops_.back();

		currentLoop.push_back(currentEdge->second);
		crossSectionEdges.erase(currentEdge);

		if (currentLoop.back().GetEndIdx() == currentLoop.front().GetStartIdx()) // loop itself
		{
			newLoop = true;
			currentEdge = crossSectionEdges.begin();
		}
		else
		{
			// int nextIdx = currentLoop.back().GetEndIdx();
			currentEdge = crossSectionEdges.find(currentLoop.back().GetEndIdx());
		}
	}

	return true;
}

bool MeshSeparator::AddCrossSectionLoopsToMesh(const std::vector<Loop>& crossSectionLoops_, Mesh& resultMesh_)
{
	int groupIDUVChannel = resultMesh_.GetUVChannelCount() - 1;

	resultMesh_.BeginAppend();

	for (auto& crossSectionLoop : crossSectionLoops_)
	{
		resultMesh_.BeginPolygon(crossSectionLoop.groupID, resultMesh_.GetMaxMaterialIdx() + 1);
		for (int i = 0; i < crossSectionLoop.size(); i++)
		{
			const Edge& edge = crossSectionLoop[i];

			Vertex vertex = resultMesh_.GetVertex(edge.GetVertexIdx());
			// const Vector3& position = resultMesh_.GetEdgeVertex(edge.GetStartIdx());
			vertex.uvs[groupIDUVChannel].X() = crossSectionLoop.groupID;
			vertex.uvs[groupIDUVChannel].Y() = 1; // is cross section

			resultMesh_.AddPolygonVertex(vertex);
		}
		resultMesh_.EndPolygon();
	}

	resultMesh_.End();

	return true;
}

bool MeshSeparator::PrintCrossSectionLoops(const Mesh& resultMesh_, const std::vector<Loop>& crossSectionLoops_)
{
	Debug::Info("Triangle Index\t");
	for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
		Debug::Info("%d\t \t \t \t", polyIdx);
	Debug::Info("\n");

	Debug::Info("Center\t");
	for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
	{
		const Vector3& center = resultMesh_.GetPolygonCenter(polyIdx);
		Debug::Info("(%2.3f,%2.3f,%2.3f)\t \t \t \t", center.X(), center.Y(), center.Z());
	}
	Debug::Info("\n");

	Debug::Info("GroupID/Adj Tri.Index\t");
	for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
	{
		int groupID = resultMesh_.GetPolygonGroupID(polyIdx);
		Debug::Info("%d\t", groupID);

		for (size_t edgeIdx = 0; edgeIdx < resultMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			const Edge& edge = resultMesh_.GetPolygonEdge(polyIdx, edgeIdx);
			Debug::Info("%d\t", edge.GetAdjacentPolygonIdx());
		}
	}
	Debug::Info("\n");

	Debug::Info("Edge Start Index\t");
	for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
	{
		int groupID = resultMesh_.GetPolygonGroupID(polyIdx);
		Debug::Info("x\t");

		for (size_t edgeIdx = 0; edgeIdx < resultMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			const Edge& edge = resultMesh_.GetPolygonEdge(polyIdx, edgeIdx);
			Debug::Info("%d\t", edge.GetStartIdx());
		}
	}
	Debug::Info("\n");

	Debug::Info("Edge End Index\t");
	for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
	{
		int groupID = resultMesh_.GetPolygonGroupID(polyIdx);
		Debug::Info("x\t");

		for (size_t edgeIdx = 0; edgeIdx < resultMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			const Edge& edge = resultMesh_.GetPolygonEdge(polyIdx, edgeIdx);
			Debug::Info("%d\t", edge.GetEndIdx());
		}
	}
	Debug::Info("\n");

	for (size_t groupID = 0; groupID < resultMesh_.GetMaxGroupIdx() + 1; groupID++)
	{
		Debug::Info("GroupID %d, Cross Section Edge Start Index\t", groupID);
		for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
		{
			for (size_t edgeIdx = 0; edgeIdx < resultMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
			{
				if (groupID != resultMesh_.GetPolygonGroupID(polyIdx))
					continue;

				int polygonEdgeAdjacentPolygonIdx = resultMesh_.GetPolygonEdgeAdjacentPolygonIdx(polyIdx, edgeIdx);
				int adjacentGroupID = resultMesh_.GetPolygonGroupID(polygonEdgeAdjacentPolygonIdx);
				if (groupID != adjacentGroupID)
				{
					const Edge& edge = resultMesh_.GetPolygonEdge(polyIdx, edgeIdx);
					Debug::Info("%d\t", edge.GetStartIdx());
				}
			}
		}
		Debug::Info("\n");

		Debug::Info("GroupID %d, Cross Section Edge End Index\t", groupID);
		for (size_t polyIdx = 0; polyIdx < resultMesh_.GetPolygonCount(); polyIdx++)
		{
			for (size_t edgeIdx = 0; edgeIdx < resultMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
			{
				if (groupID != resultMesh_.GetPolygonGroupID(polyIdx))
					continue;

				int polygonEdgeAdjacentPolygonIdx = resultMesh_.GetPolygonEdgeAdjacentPolygonIdx(polyIdx, edgeIdx);
				int adjacentGroupID = resultMesh_.GetPolygonGroupID(polygonEdgeAdjacentPolygonIdx);
				if (groupID != adjacentGroupID)
				{
					const Edge& edge = resultMesh_.GetPolygonEdge(polyIdx, edgeIdx);
					Debug::Info("%d\t", edge.GetEndIdx());
				}
			}
		}
		Debug::Info("\n");
	}

	///////////////////////////////////////////////////////////
	int i = 0;
	for (auto& crossSectionLoop : crossSectionLoops_)
	{
		Debug::Verbose("Loop %d, Groupd ID=%d\t", i++, crossSectionLoop.groupID);

		for (int i = 0; i < crossSectionLoop.size(); i++)
		{
			Debug::Verbose("%d\t", crossSectionLoop[i].GetStartIdx());
		}
		Debug::Verbose("\n");
	}

	return true;
}