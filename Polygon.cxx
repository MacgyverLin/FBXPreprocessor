#include "Polygon.h"

Polygon::Polygon()
{
	materialIdx = 0;
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
	}
}

bool Polygon::IsEmpty() const
{
	return vertices.size() == 0;
}

void Polygon::Transform(const Vector3& t, const Vector3& s)
{
	for (size_t i = 0; i < vertices.size(); i++)
	{
		vertices[i].position = vertices[i].position * s + t;
	}
}