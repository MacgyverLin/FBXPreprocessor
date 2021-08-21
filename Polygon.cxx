#include "Polygon.h"

Polygon::Polygon(int materialIdx_)
{
	this->materialIdx = materialIdx_;
}

Polygon::Polygon(int materialIdx, const std::vector<Vertex>& vertices)
{
	this->materialIdx = materialIdx;
	this->vertices = vertices;
}

Polygon::~Polygon()
{
}

Polygon::Polygon(const Polygon& other)
{
	materialIdx = other.materialIdx;
	vertices = other.vertices;
}

Polygon& Polygon::operator = (const Polygon& other)
{
	materialIdx = other.materialIdx;
	vertices = other.vertices;

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

/*
const std::vector<Vertex>& Polygon::GetVertices() const
{
	return vertices;
}

std::vector<Vertex>& Polygon::GetVertices()
{
	return vertices;
}
*/

void Polygon::Add(const Vertex& vertex)
{
	vertices.push_back(vertex);
}

void Polygon::Clear()
{
	materialIdx = 0;
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

Plane Polygon::GetPlane() const
{
	return Plane(vertices[0].position, vertices[1].position, vertices[2].position);
}

void Triangulate(const Polygon& polygon, std::vector<Polygon>& polygons)
{
	if (polygon.vertices.size() < 3)
		return;

	int startIdx = polygons.size();
	polygons.resize(polygons.size() + polygon.vertices.size() - 2);
	for (size_t i = 0; i < polygon.vertices.size() - 2; i++)
	{
		polygons[startIdx + i].materialIdx = polygon.materialIdx;

		polygons[startIdx + i].vertices.resize(3);
		polygons[startIdx + i].vertices[0] = polygon.vertices[0];
		polygons[startIdx + i].vertices[1] = polygon.vertices[i + 1];
		polygons[startIdx + i].vertices[2] = polygon.vertices[i + 2];
	}
}