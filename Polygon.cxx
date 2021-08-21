#include "Polygon.h"

Polygon::Polygon(int materialIdx_)
	: materialIdx(materialIdx_)
	, plane()
	, aabb()
	, vertices()
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
}

Polygon& Polygon::operator = (const Polygon& other)
{
	materialIdx = other.materialIdx;
	plane = other.plane;
	aabb = other.aabb;
	vertices = other.vertices;

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

void Polygon::Begin(int materialIdx_)
{
	materialIdx = materialIdx_;
	plane = Plane();
	aabb = AABB();
	vertices.clear();
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

void Triangulate(const Polygon& polygon, std::vector<Polygon>& polygons)
{
	if (polygon.GetVerticesCount() < 3)
		return;

	int startIdx = polygons.size();
	polygons.resize(polygons.size() + polygon.GetVerticesCount() - 2);

	for (size_t i = 0; i < polygon.GetVerticesCount() - 2; i++)
	{
		polygons[startIdx + i].Begin(polygon.GetMaterialIdx());
		
		polygons[startIdx + i].Add(polygon.GetVertex(0));
		polygons[startIdx + i].Add(polygon.GetVertex(i + 1));
		polygons[startIdx + i].Add(polygon.GetVertex(i + 2));

		polygons[startIdx + i].End();

		/*
		polygons[startIdx + i].materialIdx = polygon.materialIdx;

		polygons[startIdx + i].vertices.resize(3);
		polygons[startIdx + i].vertices[0] = polygon.vertices[0];
		polygons[startIdx + i].vertices[1] = polygon.vertices[i + 1];
		polygons[startIdx + i].vertices[2] = polygon.vertices[i + 2];
		*/
	}
}