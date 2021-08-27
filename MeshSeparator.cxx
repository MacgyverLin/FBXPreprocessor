#include "MeshSeparator.h"
#include "Polygon.h"
#include "BSP.h"

MeshSeparator::MeshSeparator()
{
}

MeshSeparator::~MeshSeparator()
{
}

bool MeshSeparator::Process(const std::vector<Mesh>& srcMeshes_, BSP* root_, std::vector<MeshArray>& resultMeshArrays_)
{
	resultMeshArrays_.resize(srcMeshes_.size());
	for (size_t i = 0; i < srcMeshes_.size(); i++)
	{
		resultMeshArrays_[i].resize(1);

		if (!Process(srcMeshes_[i], root_, resultMeshArrays_[i][0]))
			return false;
	}

	return true;
}

bool MeshSeparator::Process(const Mesh& srcMesh_, BSP* root_, Mesh& resultMesh_)
{
	// make sure src Mesh is closed
	if (!srcMesh_.IsClosed())
		return false;

	// slice, use bsp to set groupID
	if (!Slice(srcMesh_, root_, resultMesh_))
		return false;

	std::vector<Loop> crossSectionLoops;
	if(!BuildCrossSectionLoops(resultMesh_, crossSectionLoops))
		return false;

	if (!PrintCrossSectionLoops(crossSectionLoops))
		return false;

	if (!AddCrossSectionLoopsToMesh(crossSectionLoops, resultMesh_))
		return false;

	return true;
}

bool MeshSeparator::Slice(const Mesh& srcMesh_, BSP* root_, Mesh& resultMesh_)
{
	Plane splitPlane
	(
		//srcMesh_.GetAABB().GetMajorAxis(true),
		//srcMesh_.GetAABB().GetCenter(true)
		Vector3::UnitY,
		srcMesh_.GetAABB().GetCenter(true)
	);

	resultMesh_.Begin(srcMesh_.GetColorChannelCount(), srcMesh_.GetUVChannelCount(), srcMesh_.GetNormalChannelCount(), srcMesh_.GetTangentChannelCount(), srcMesh_.GetBinormalChannelCount());

	for (size_t polyIdx = 0; polyIdx < srcMesh_.GetPolygonCount(); polyIdx++)
	{
		const Vector3& center = srcMesh_.GetPolygonCenter(polyIdx);
		int polygonGroupID = GetGroupIdx(srcMesh_, splitPlane, root_, center);

		resultMesh_.BeginPolygon(polygonGroupID, srcMesh_.GetPolygonMaterialIdx(polyIdx));
		for (size_t edgeIdx = 0; edgeIdx < srcMesh_.GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			const Vertex& v = srcMesh_.GetPolygonVertex(polyIdx, edgeIdx);
			resultMesh_.AddPolygonVertex(v);
		}
		resultMesh_.EndPolygon();
	}

	resultMesh_.End();

	return true;
}

int MeshSeparator::GetGroupIdx(const Mesh& srcMesh_, const Plane& splitPlane_, BSP* root_, const Vector3& p_)
{
	int polygonGroupID = 0;
	if (splitPlane_.WhichSide(p_))
		polygonGroupID = 0;
	else
		polygonGroupID = 1;

	return polygonGroupID;
}

bool MeshSeparator::BuildCrossSectionLoops(const Mesh& resultMesh_, std::vector<Loop>& crossSectionLoops_)
{
	for (int groupID = 0; groupID <= resultMesh_.GetMaxGroupIdx(); groupID++)
	{
		Debug::Verbose("Polygon Group=%d\n", groupID);
		Debug::Verbose("{\n");

		std::multimap<int, Edge> crossSectionEdges;
		if (!BuildCrossSectionEdges(resultMesh_, groupID, crossSectionEdges))
			return false;

		if (!BuildCrossSectionLoopsFromEdges(crossSectionEdges, groupID, crossSectionLoops_))
			return false;

		Debug::Verbose("}\n\n");
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

			if (resultMesh_.GetPolygonGroupID(polyIdx) != resultMesh_.GetPolygonGroupID(polygonEdgeAdjacentPolygonIdx))
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
	int maxMaterialIdx = resultMesh_.GetMaxMaterialIdx();

	resultMesh_.BeginAppend();

	for (auto& crossSectionLoop : crossSectionLoops_)
	{
		resultMesh_.BeginPolygon(crossSectionLoop.groupID, maxMaterialIdx + 1);
		for (int i = 0; i < crossSectionLoop.size(); i++)
		{
			const Edge& edge = crossSectionLoop[i];

			const Vertex& vertex = resultMesh_.GetVertex(edge.GetVertexIdx());
			const Vector3& position = resultMesh_.GetEdgeVertex(edge.GetStartIdx());

			resultMesh_.AddPolygonVertex(vertex);
		}
		resultMesh_.EndPolygon();
	}

	resultMesh_.End();

	return true;
}

bool MeshSeparator::PrintCrossSectionLoops(const std::vector<Loop>& crossSectionLoops_)
{
	for (auto& crossSectionLoop : crossSectionLoops_)
	{
		Debug::Verbose("\tLoop\n");
		Debug::Verbose("\t{\n");
		Debug::Verbose("\t\tGroupd ID=%d\n", crossSectionLoop.groupID);
		for (int i = 0; i < crossSectionLoop.size(); i++)
		{
			Debug::Verbose("\t\t(%d,%d)", crossSectionLoop[i].GetStartIdx(), crossSectionLoop[i].GetEndIdx());

			if (i == 0)
			{
				if (crossSectionLoop[crossSectionLoop.size() - 1].GetEndIdx() == crossSectionLoop[0].GetStartIdx())
					Debug::Verbose(" *");
			}
			else
			{
				if (crossSectionLoop[i - 1].GetEndIdx() == crossSectionLoop[i].GetStartIdx())
					Debug::Verbose(" *");
			}

			Debug::Verbose("  \n");
		}
		Debug::Verbose("\t}\n");
	}
	return true;
}