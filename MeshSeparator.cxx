#include "MeshSeparator.h"
#include "Polygon.h"
#include "BSP.h"

// material
// cross section material
// shadow problem
// normal
// vertex explode

MeshSeparator::MeshSeparator()
{
}

MeshSeparator::~MeshSeparator()
{
}

bool MeshSeparator::Process(const std::vector<Mesh>& srcMeshes_, std::vector<MeshArray>& resultMeshArrays_)
{
	int sceneMaxMaterialIdx = ComputeSceneMaxMaterialIdx(srcMeshes_);

	resultMeshArrays_.resize(srcMeshes_.size());
	for (size_t i = 0; i < srcMeshes_.size(); i++)
	{
		resultMeshArrays_[i].resize(1);

		if (!Process(sceneMaxMaterialIdx, srcMeshes_[i], resultMeshArrays_[i][0]))
			return false;
	}

	return true;
}

bool MeshSeparator::Process(int sceneMaxMaterialIdx_, const Mesh& srcMesh_, Mesh& resultMesh_)
{
#if defined(USE_METHOD1)
	BSP bsp;

	for (int i = 0; i < 10; i++)
	{
		bsp.Split
		(
			Plane
			(
				srcMesh_.GetAABB().GetMajorAxis(true),
				srcMesh_.GetAABB().GetCenter(true)
				// Vector3::UnitY,
				// srcMesh_.GetAABB().GetCenter(false)
			)
		);
	}

	// make sure src Mesh is closed
	if (!srcMesh_.IsClosed())
		return false;

	// slice, use bsp to set groupID
	if (!Slice(srcMesh_, bsp, resultMesh_))
		return false;

	//std::vector<Loop2> crossSectionLoops2;
	//if (!BuildCrossSectionLoops2(resultMesh_, crossSectionLoops2))
		//return false;

	//if (!PrintCrossSectionLoops2(resultMesh_, crossSectionLoops2))
//		return false;

	//if (!AddCrossSectionLoopsToMesh2(crossSectionLoops2, sceneMaxMaterialIdx_, 1.0f, resultMesh_))
		//return false;
#elif defined(USE_METHOD2)
#endif
	return true;
}

#if defined(USE_METHOD1)

bool MeshSeparator::Slice(const Mesh& srcMesh_, BSP& bsp_, Mesh& resultMesh_)
{
	std::vector<const BSP*> bspArray;
	bsp_.BFS
	(
		[&](const BSP* node) -> void
		{
			bspArray.push_back(node);
		}
	);

	std::map<int, int> groupIDMaps;

	int uvChannelCount = srcMesh_.GetUVChannelCount();
	resultMesh_.Begin(srcMesh_.GetColorChannelCount(), uvChannelCount + 1, srcMesh_.GetNormalChannelCount(), srcMesh_.GetTangentChannelCount(), srcMesh_.GetBinormalChannelCount());

	int groupIDUVChannel = resultMesh_.GetUVChannelCount() - 1;
	for (size_t polyIdx = 0; polyIdx < srcMesh_.GetPolygonCount(); polyIdx++)
	{
		const Vector3& center = srcMesh_.GetPolygonCenter(polyIdx);
		int region = GetBSPRegion(srcMesh_, bspArray, center);
		if(groupIDMaps.find(region) == groupIDMaps.end())
			groupIDMaps[region] = groupIDMaps.size();
		
		int groupID = groupIDMaps[region];
		Debug::Info("%d->%d\n", region, groupID);

		resultMesh_.BeginPolygon(groupID, srcMesh_.GetPolygonMaterialIdx(polyIdx));
		for (size_t edgeIdx = 0; edgeIdx < srcMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			Vertex v = srcMesh_.GetPolygonVertex(polyIdx, edgeIdx);
			v.uvs[groupIDUVChannel] = Vector2(groupID, 0); // not a cross section vertex

			resultMesh_.AddPolygonVertex(v);
		}
		resultMesh_.EndPolygon();
	}

	resultMesh_.End();

	return true;
}

int MeshSeparator::ComputeSceneMaxMaterialIdx(const std::vector<Mesh>& srcMeshes_)
{
	int sceneMaxMaterialIdx = 0;
	for (size_t i = 0; i < srcMeshes_.size(); i++)
	{
		if (sceneMaxMaterialIdx < srcMeshes_[i].GetMaxMaterialIdx())
			sceneMaxMaterialIdx = srcMeshes_[i].GetMaxMaterialIdx();
	}

	return sceneMaxMaterialIdx;
}

int MeshSeparator::GetBSPRegion(const Mesh& srcMesh_, const std::vector<const BSP*>& bspArray_, const Vector3& p_)
{
	int idx = 0;

	while ((idx * 2 + 2) < bspArray_.size())
	{
		const Plane& plane = bspArray_[idx]->GetPlane();
		
		if (plane.WhichSide(p_))
		{
			idx = idx * 2 + 1;
		}
		else
		{
			idx = idx * 2 + 2;
		}
	}

	return idx;

	/*
	int polygonGroupID = 0;
	if (root_->GetPlane().WhichSide(p_))
	{
		if (p_.Z() > 0.5f)
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
	*/
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

				unorderedEdges.insert
				(
					std::pair<int, int>
					(
						edge.GetStartIdx(),
						edge.GetEndIdx()
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
		//for (UnorderedEdges::iterator& unorderedEdgesItr = unorderedEdges_.begin(); unorderedEdgesItr != unorderedEdges_.end(); unorderedEdgesItr++)
		//{
			//if (!unorderedEdgesItr->second)
				//return unorderedEdgesItr;
		//}
		return unorderedEdges_.begin();
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

				int nextVertex = currentEdgeItr->second;
				unorderedEdges.erase(currentEdgeItr);

				currentEdgeItr = unorderedEdges.find(nextVertex);
			} while (currentEdgeItr != unorderedEdges.end());

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
	int loop = 0;
	for (auto& crossSectionLoop : crossSectionLoops_)
	{
		Debug::Verbose("Loop %d, Groupd ID=%d\t", loop++, crossSectionLoop.groupID);

		for (int i = 0; i < crossSectionLoop.size(); i++)
		{
			Debug::Verbose("%d\t", crossSectionLoop[i]);
		}
		Debug::Verbose("\n");
	}


	///////////////////////////////////////////////////////////
	loop = 0;
	for (auto& crossSectionLoop : crossSectionLoops_)
	{
		Debug::Verbose("Loop %d, Groupd ID=%d\t", loop++, crossSectionLoop.groupID);

		std::map<Vector3, int> vmap;

		for (int i = 0; i < crossSectionLoop.size(); i++)
		{
			const Vector3& v = resultMesh_.GetEdgeStartVertex(crossSectionLoop[i]);
			//if (vmap.find(v) != vmap.end())
				//return false;

			vmap[v] = i;
			Debug::Info("(%2.3f,%2.3f,%2.3f)->", v.X(), v.Y(), v.Z());
		}
		Debug::Verbose("\n");
	}

	return true;
}

bool MeshSeparator::AddCrossSectionLoopsToMesh2(const std::vector<Loop2>& crossSectionLoops_, int sceneMaxMaterialIdx_, float textureSize_, Mesh& resultMesh_)
{
	resultMesh_.BeginAppend();

	for (auto& crossSectionLoop : crossSectionLoops_)
	{
		if (crossSectionLoop.size() < 2)
			continue;

		Vector3 center(Vector3::Zero);
		for (int i = 0; i < crossSectionLoop.size(); i++)
		{
			const int& edgeVertexIdx = crossSectionLoop[i];
			center += resultMesh_.GetEdgeStartVertex(edgeVertexIdx);
		}
		center /= crossSectionLoop.size();

		const Vector3& v0 = resultMesh_.GetEdgeStartVertex(crossSectionLoop[1]);
		const Vector3& v1 = resultMesh_.GetEdgeStartVertex(crossSectionLoop[0]);
		Vector3 tangent(v0 - center); 
		tangent.Normalize();
		Vector3 binormal(v1 - center); 
		binormal = binormal - binormal.Dot(tangent) * tangent;
		binormal.Normalize();
		Vector3 normal(tangent.Cross(binormal));

		Matrix4 projMatrix;
		projMatrix.Init(tangent, binormal, normal, center);
		projMatrix = projMatrix.Inverse();

		Vertex apex = MakeVertex(resultMesh_, projMatrix, center - normal * 0.05f, normal, tangent, binormal, 1.0f, crossSectionLoop.groupID);

		for (int i = 0; i < crossSectionLoop.size(); i++)
		{
			resultMesh_.BeginPolygon(crossSectionLoop.groupID, sceneMaxMaterialIdx_ + 1);
			//resultMesh_.BeginPolygon(crossSectionLoop.groupID, (crossSectionLoop.groupID + 1) * 10);

			const int& edgeVertexIdx0 = crossSectionLoop[(i + 1) % crossSectionLoop.size()];
			const int& edgeVertexIdx1 = crossSectionLoop[(i + 0) % crossSectionLoop.size()];
			const Vector3& p0 = resultMesh_.GetEdgeStartVertex(edgeVertexIdx0);
			const Vector3& p1 = resultMesh_.GetEdgeStartVertex(edgeVertexIdx1);
			Vertex v0 = MakeVertex(resultMesh_, projMatrix, p0, normal, tangent, binormal, 1.0f, crossSectionLoop.groupID);
			Vertex v1 = MakeVertex(resultMesh_, projMatrix, p1, normal, tangent, binormal, 1.0f, crossSectionLoop.groupID);

			resultMesh_.AddPolygonVertex(apex);
			resultMesh_.AddPolygonVertex(v0);
			resultMesh_.AddPolygonVertex(v1);

			resultMesh_.EndPolygon();
		}
	}

	resultMesh_.End();

	return true;
}

Vertex MeshSeparator::MakeVertex(const Mesh& mesh, const Matrix4& projMatrix, 
				const Vector3& position, 
				const Vector3& normal, const Vector3& tangent, const Vector3& binormal, 
				float textureSize, int groupID)
{
	Vertex v;
	v.position = position;
	Debug::Info("%3.2f, %3.2f, %3.2f\n", position.X(), position.Y(), position.Z());

	Vector3 vProj = projMatrix.TimesPositionVector(v.position);
	for (size_t i = 0; i < mesh.GetColorChannelCount(); i++)
		v.colors[i] = Color(1.0, 1.0, 1.0, 1.0);

	Vector2 uv = Vector2(vProj.X(), vProj.Y());
	uv *= Vector2(1.0f / textureSize, 1.0f / textureSize);

	Debug::Info("%3.2f, %3.2f\n", uv.X(), uv.Y());
	for (size_t i = 0; i < mesh.GetUVChannelCount() - 1; i++)
		v.uvs[i] = uv;
	
	int groupIDUVChannel = mesh.GetUVChannelCount() - 1;
	v.uvs[groupIDUVChannel] = Vector2(groupID, 1); // is cross section

	for (size_t i = 0; i < mesh.GetNormalChannelCount(); i++)
		v.normals[i] = normal;

	for (size_t i = 0; i < mesh.GetTangentChannelCount(); i++)
		v.tangents[i] = tangent;

	for (size_t i = 0; i < mesh.GetBinormalChannelCount(); i++)
		v.binormals[i] = binormal;

	return v;
}

#elif defined(USE_METHOD2)

bool MeshSeparator::Group(const Mesh& srcMesh_, Mesh& resultMesh_, int groupCount_)
{
	int uvChannelCount = srcMesh_.GetUVChannelCount();

	resultMesh_.Begin(srcMesh_.GetColorChannelCount(), uvChannelCount + 1, srcMesh_.GetNormalChannelCount(), srcMesh_.GetTangentChannelCount(), srcMesh_.GetBinormalChannelCount());
	int groupIDUVChannel = resultMesh_.GetUVChannelCount() - 1;

	int maxTrianglesPerGroup = srcMesh_.GetPolygonCount() / groupCount_;

	for (size_t polyIdx = 0; polyIdx < srcMesh_.GetPolygonCount(); polyIdx++)
	{
	}

	for (size_t polyIdx = 0; polyIdx < srcMesh_.GetPolygonCount(); polyIdx++)
	{
		const Vector3& center = srcMesh_.GetPolygonCenter(polyIdx);
		int groupID = GetSlicedGroupIdx(srcMesh_, root_, center);

		if (groupID == 1)
		{
			groupID = groupID;
		}

		resultMesh_.BeginPolygon(groupID, srcMesh_.GetPolygonMaterialIdx(polyIdx));
		for (size_t edgeIdx = 0; edgeIdx < srcMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			Vertex v = srcMesh_.GetPolygonVertex(polyIdx, edgeIdx);
			v.uvs[groupIDUVChannel] = Vector2(groupID, 0); // not a cross section vertex

			resultMesh_.AddPolygonVertex(v);
		}
		resultMesh_.EndPolygon();
	}

	resultMesh_.End();
}

#endif