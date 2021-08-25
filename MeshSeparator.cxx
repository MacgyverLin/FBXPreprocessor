#include "MeshSeparator.h"
#include "Polygon.h"

MeshSeparator::MeshSeparator()
{
}

MeshSeparator::~MeshSeparator()
{
}

bool MeshSeparator::Process(const std::vector<Mesh>& srcMeshes, std::vector<MeshArray>& resultMeshArrays)
{
	resultMeshArrays.resize(srcMeshes.size());
	for (size_t i = 0; i < srcMeshes.size(); i++)
	{
		MeshArray& resultMeshArray = resultMeshArrays[i];
		resultMeshArray.resize(1);

		Mesh& resultMesh = resultMeshArray[0];
		if (!Process(srcMeshes[i], resultMesh))
			return false;
	}

	return true;
}

bool MeshSeparator::Process(const Mesh& srcMesh, Mesh& resultMesh)
{
	// make sure src Mesh is closed
	if (!srcMesh.IsClosed())
		return false;

	resultMesh = srcMesh;

	// group
	Vector3 center = resultMesh.GetAABB().GetCenter(true);
	//Vector3 normal = GetAABB().GetMajorAxis(true);
	Vector3 normal = Vector3::UnitY;
	Plane cutPlane(center, normal);

	for (size_t polyIdx = 0; polyIdx < resultMesh.GetPolygonCount(); polyIdx++)
	{
		Vector3 polyCenter = resultMesh.GetPolygonAABB(polyIdx).GetCenter();

		if (cutPlane.WhichSide(polyCenter))
		{
			resultMesh.SetPolygonGroupID(polyIdx, 0);
		}
		else
		{
			resultMesh.SetPolygonGroupID(polyIdx, 1);
		}
	}


	for (int groupID = 0; groupID <= resultMesh.GetMaxGroupIdx(); groupID++)
	{
		Debug::Verbose("Polygon Group=%d\n", groupID);
		Debug::Verbose("{\n");

		std::multimap<int, Edge> isolatedEdges;
		{
			// if polygon neighbour ID is not myself, save the edge in the map
			for (size_t polyIdx = 0; polyIdx < resultMesh.GetPolygonCount(); polyIdx++)
			{
				if (groupID != resultMesh.GetPolygonGroupID(polyIdx))
					continue;

				for (size_t edgeIdx = 0; edgeIdx < resultMesh.GetPolygonEdgesCount(polyIdx); edgeIdx++)
				{
					int polygonEdgeAdjacentPolygonIdx = resultMesh.GetPolygonEdgeAdjacentPolygonIdx(polyIdx, edgeIdx);

					if (resultMesh.GetPolygonGroupID(polyIdx) != resultMesh.GetPolygonGroupID(polygonEdgeAdjacentPolygonIdx))
						//if (polygon.GetGroupID() != adjacentPolygon.GetGroupID())
					{
						const Edge& edge = resultMesh.GetPolygonEdge(polyIdx, edgeIdx);
						isolatedEdges.insert(std::pair<int, Edge>(edge.GetStartIdx(), edge));
					}
				}
			}
		}

		typedef std::vector<Edge> Loop;
		std::vector<Loop> loops;
		{
			bool newLoop = true;

			std::multimap<int, Edge>::iterator currentEdge = isolatedEdges.begin();
			while (isolatedEdges.size())
			{
				if (newLoop)
				{
					loops.push_back(Loop());
					newLoop = false;
				}

				Loop& currentLoop = loops.back();

				currentLoop.push_back(currentEdge->second);
				isolatedEdges.erase(currentEdge);

				if (currentLoop.back().GetEndIdx() == currentLoop.front().GetStartIdx()) // loop itself
				{
					newLoop = true;
					currentEdge = isolatedEdges.begin();
				}
				else
				{
					int nextIdx = currentLoop.back().GetEndIdx();
					currentEdge = isolatedEdges.find(currentLoop.back().GetEndIdx());
				}
			}
		}

		for (auto& loop : loops)
		{
			Debug::Verbose("\tLoop\n");
			Debug::Verbose("\t{\n");
			for (int i = 0; i < loop.size(); i++)
			{
				Debug::Verbose("\t\t(%d,%d)", loop[i].GetStartIdx(), loop[i].GetEndIdx());

				if (i == 0)
				{
					if (loop[loop.size() - 1].GetEndIdx() == loop[0].GetStartIdx())
						Debug::Verbose(" *");
				}
				else
				{
					if (loop[i - 1].GetEndIdx() == loop[i].GetStartIdx())
						Debug::Verbose(" *");
				}

				Debug::Verbose("  \n");
			}
			Debug::Verbose("\t}\n");
		}

		Debug::Verbose("}\n\n");

		/*
		for (auto& loop : loops)
		{
			BeginPolygon(GetMaxGroupIdx()+1, GetMaxMaterialIdx() + 1);
			for (int i = 0; i < loop.size(); i++)
			{
				const Edge& edge = loop[i];

				const Vertex& vertex = verticesOptimizer.GetData(edge.GetVertexIdx());
				AddPolygonVertex(vertex);
			}
			EndPolygon();
		}
		*/
	}

	return true;
}