#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

Mesh::Mesh(const Mesh& other)
{
	polygons = other.polygons;
}

Mesh& Mesh::operator = (const Mesh& other)
{
	polygons = other.polygons;

	return *this;
}

void Mesh::Flip()
{
	for (size_t i = 0; i < polygons.size(); i++)
	{
		polygons[i].Flip();
	}
}

bool Mesh::IsEmpty() const
{
	return polygons.size() == 0;
}

void Mesh::Transform(const Vector3& t, const Vector3& s)
{
	for (size_t i = 0; i < polygons.size(); i++)
	{
		polygons[i].Transform(t, s);
	}
}

void Mesh::Add(const Polygon& polygon)
{
	polygons.push_back(polygon);
}