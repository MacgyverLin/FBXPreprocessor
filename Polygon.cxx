#include "Polygon.h"

Polygon::Polygon(int groupIdx_, int materialIdx_)
	: materialIdx(materialIdx_)
	, vertices()

	, groupIdx(groupIdx_)
	, center()
	, plane()
	, aabb()
	, adjacencies()
{
}

Polygon::~Polygon()
{
}

Polygon::Polygon(const Polygon& other)
{
	materialIdx = other.materialIdx;
	vertices = other.vertices;

	groupIdx = other.groupIdx;
	center = other.center;
	plane = other.plane;
	aabb = other.aabb;
	adjacencies = other.adjacencies;
}

Polygon& Polygon::operator = (const Polygon& other)
{
	materialIdx = other.materialIdx;
	vertices = other.vertices;

	groupIdx = other.groupIdx;
	center = other.center;
	plane = other.plane;
	aabb = other.aabb;
	adjacencies = other.adjacencies;

	return *this;
}

int Polygon::GetMaterialIdx() const
{
	return materialIdx;
}

int Polygon::GetVerticesCount() const
{
	return vertices.size();
}

const Vertex& Polygon::GetVertex(int i) const
{
	return vertices[i];
}

const std::vector<Vertex>& Polygon::GetVertices() const
{
	return vertices;
}

int Polygon::GetGroupIdx() const
{
	return groupIdx;
}

const Vector3& Polygon::GetCenter() const
{
	return center;
}

const Plane& Polygon::GetPlane() const
{
	return plane;
}

const AABB& Polygon::GetAABB() const
{
	return aabb;
}

const Adjacency& Polygon::GetAdjacency(int i) const
{
	return adjacencies[i];
}

const std::vector<Adjacency>& Polygon::GetAdjacencies() const
{
	return adjacencies;
}

void Polygon::Begin(int groupIdx_, int materialIdx_)
{
	materialIdx = materialIdx_;
	vertices.clear();

	groupIdx = groupIdx_;
	center = Vector3::Zero;
	plane = Plane(Vector3::UnitY, 0);
	aabb = AABB();
	adjacencies.clear();
}

void Polygon::Add(const std::vector<Vertex>& verts)
{
	for (auto& v : verts)
		Add(v);
}

void Polygon::Add(const Vertex& vertex)
{
	vertices.push_back(vertex);
}

void Polygon::End()
{
	//////////////////////////////////////
	// compute Center
	if (vertices.size()==0)
	{
		center = Vector3::Zero;
	}
	else
	{
		center = Vector3::Zero;
		for (size_t i = 0; i < vertices.size(); i++)
			center += vertices[i].position;

		center /= vertices.size();
	}

	//////////////////////////////////////
	// compute Plane
	if (vertices.size() >= 3)
		plane = Plane(vertices[0].position, vertices[1].position, vertices[2].position);

	//////////////////////////////////////
	// compute AABB
	if (vertices.size()==0)
	{
		aabb = AABB(Vector3::Zero, Vector3::Zero);
	}
	else
	{
		aabb = AABB(vertices[0].position, vertices[0].position);
		for (size_t i = 0; i < vertices.size(); i++)
			aabb += vertices[i].position;
	}
}

void Polygon::End(DataOptimizer<Vector3>& positionOptimizer, DataOptimizer<Edge>& edgeOptimizer)
{
	End();

	//////////////////////////////////////
	// compute Adjacency
	for (size_t i = 0; i < vertices.size(); i++)
	{
		int i0 = (i + 0) % vertices.size();
		int i1 = (i + 1) % vertices.size();

		int vIdx0 = positionOptimizer.Add(vertices[i0].position);
		int vIdx1 = positionOptimizer.Add(vertices[i1].position);
		int edgeIdx = edgeOptimizer.Add(Edge(vIdx0, vIdx1));

		adjacencies.push_back(Adjacency((vIdx1 > vIdx0), edgeIdx));
	}
}

void Polygon::Clear()
{
	materialIdx = 0;
	vertices.clear();

	groupIdx = 0;
	center = Vector3::Zero;
	plane = Plane(Vector3::UnitY, 0);
	aabb = AABB(Vector3::Zero, Vector3::Zero);
	adjacencies.clear();
}

void Polygon::Flip()
{
	std::vector<Vertex> oldVertices = vertices;
	for (size_t i = 0; i < vertices.size(); i++)
	{
		vertices[i] = oldVertices[oldVertices.size() - 1 - i];
		vertices[i].Flip();
	}

	for (size_t i = 0; i < adjacencies.size(); i++)
		adjacencies[i].FlipEdge();
}

bool Polygon::IsEmpty() const
{
	return vertices.size() == 0;
}