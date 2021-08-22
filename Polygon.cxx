#include "Polygon.h"

Polygon::Polygon(int materialIdx_)
	: materialIdx(materialIdx_)
	, plane()
	, aabb()
	, vertices()
	, flipEdges()
{
}

Polygon::~Polygon()
{
}

Polygon::Polygon(const Polygon& other)
{
	materialIdx = other.materialIdx;
	plane = other.plane;
	aabb = other.aabb;
	vertices = other.vertices;
	flipEdges = other.flipEdges;
}

Polygon& Polygon::operator = (const Polygon& other)
{
	materialIdx = other.materialIdx;
	plane = other.plane;
	aabb = other.aabb;
	vertices = other.vertices;
	flipEdges = other.flipEdges;

	return *this;
}

int Polygon::GetMaterialIdx() const
{
	return materialIdx;
}

const Plane& Polygon::GetPlane() const
{
	return plane;
}

const AABB& Polygon::GetAABB() const
{
	return aabb;
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

int Polygon::GetFlipEdgesCount() const
{
	return flipEdges.size();
}

const FlipEdge& Polygon::GetFlipEdge(int i) const
{
	return flipEdges[i];
}

const std::vector<FlipEdge>& Polygon::GetFlipEdges() const
{
	return flipEdges;
}

void Polygon::Begin(int materialIdx_)
{
	materialIdx = materialIdx_;
	plane = Plane();
	aabb = AABB();
	vertices.clear();
	flipEdges.clear();
}

void Polygon::Add(const std::vector<Vertex>& verts)
{
	for (auto& v : verts)
		Add(v);
}

void Polygon::Add(const Vertex& vertex)
{
	if (vertices.size() == 0)
		aabb = AABB(vertex.position, vertex.position);
	else
		aabb += vertex.position;

	vertices.push_back(vertex);
}

void Polygon::Add(const FlipEdge& edge)
{
	flipEdges.push_back(edge);
}

void Polygon::End()
{
	// Todo: handle degenerate polygon
	if(vertices.size()>=3)
		plane = Plane(vertices[0].position, vertices[1].position, vertices[2].position);
}

void Polygon::Clear()
{
	materialIdx = 0;
	plane = Plane();
	aabb = AABB();
	vertices.clear();
}

void Polygon::Flip()
{
	std::vector<Vertex> oldVertices = vertices;
	for (size_t i = 0; i < vertices.size(); i++)
	{
		vertices[i] = oldVertices[oldVertices.size() - 1 - i];
		vertices[i].Flip();
	}
}

bool Polygon::IsEmpty() const
{
	return vertices.size() == 0;
}