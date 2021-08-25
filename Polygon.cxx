#include "Polygon.h"

Polygon::Polygon(int groupIdx_, int materialIdx_)
	: materialIdx(materialIdx_)
	, edges()

	, groupID(groupIdx_)
	, plane()
	, aabb()
{
}

Polygon::~Polygon()
{
}

Polygon::Polygon(const Polygon& other)
{
	materialIdx = other.materialIdx;
	edges = other.edges;

	groupID = other.groupID;
	plane = other.plane;
	aabb = other.aabb;
}

Polygon& Polygon::operator = (const Polygon& other)
{
	materialIdx = other.materialIdx;
	edges = other.edges;

	groupID = other.groupID;
	plane = other.plane;
	aabb = other.aabb;

	return *this;
}

int Polygon::GetMaterialIdx() const
{
	return materialIdx;
}

int Polygon::GetEdgesCount() const
{
	return edges.size();
}

const Edge& Polygon::GetEdge(int i) const
{
	return edges[i];
}

const std::vector<Edge>& Polygon::GetEdges() const
{
	return edges;
}

int Polygon::GetGroupID() const
{
	return groupID;
}

const Plane& Polygon::GetPlane() const
{
	return plane;
}

const AABB& Polygon::GetAABB() const
{
	return aabb;
}

void Polygon::SetGroupID(int groupID_)
{
	groupID = groupID_;
}

void Polygon::Begin(int groupIdx_, int materialIdx_)
{
	materialIdx = materialIdx_;
	edges.clear();

	groupID = groupIdx_;
	plane = Plane(Vector3::UnitY, 0);
	aabb = AABB();
}

void Polygon::Add(int vertexIdx, int startIdx, int adjacentPolygonIdx)
{
	if (edges.size())
		edges.back().SetEndIdx(startIdx);

	edges.push_back(Edge(vertexIdx, startIdx, adjacentPolygonIdx));
}

void Polygon::End(DataOptimizer<Vertex>& verticesOptimizer_, DataOptimizer<Vector3>& edgeVertexOptimizer_)
{
	//////////////////////////////////////
	// close polygon loop
	if (edges.size())
		edges.back().SetEndIdx(edges.front().GetStartIdx());

	//////////////////////////////////////
	// compute Plane
	if (edges.size() >= 3)
	{
		plane = Plane
		(
			edgeVertexOptimizer_.GetData(edges[0].GetStartIdx()),
			edgeVertexOptimizer_.GetData(edges[1].GetStartIdx()),
			edgeVertexOptimizer_.GetData(edges[2].GetStartIdx())
		);
	}

	//////////////////////////////////////
	// compute AABB
	if (edges.size()==0)
	{
		aabb = AABB(Vector3::Zero, Vector3::Zero);
	}
	else
	{
		aabb = AABB
		(
			edgeVertexOptimizer_.GetData(edges[0].GetStartIdx()), 
			edgeVertexOptimizer_.GetData(edges[0].GetStartIdx())
		);
		for (size_t i = 0; i < edges.size(); i++)
		{
			aabb += edgeVertexOptimizer_.GetData(edges[i].GetStartIdx());
		}
	}
}

void Polygon::SetEdgeAdjacentPolygonIdx(int edgeIdx, int adjacentPolygonIdx)
{
	edges[edgeIdx].SetAdjacentPolygonIdx(adjacentPolygonIdx);
}

void Polygon::Clear()
{
	materialIdx = 0;
	edges.clear();

	groupID = 0;
	plane = Plane(Vector3::UnitY, 0);
	aabb = AABB(Vector3::Zero, Vector3::Zero);
}

void Polygon::Flip()
{
	std::vector<Edge> oldEdges = edges;
	for (size_t i = 0; i < oldEdges.size(); i++)
	{
		edges[i] = oldEdges[oldEdges.size() - 1 - i];
		edges[i].Flip();
	}
}

bool Polygon::IsEmpty() const
{
	return edges.size() == 0;
}