#include "Mesh.h"
#include "BSP.h"

Mesh::Mesh()
{
	maxMaterialIdx = 0;

	colorChannelCount = 0;
	uvChannelCount = 0;
	normalChannelCount = 0;
	tangentChannelCount = 0;
	binormalChannelCount = 0;
}

Mesh::~Mesh()
{
}

Mesh::Mesh(const Mesh& other)
{
	maxMaterialIdx = other.maxMaterialIdx;

	colorChannelCount = other.colorChannelCount;
	uvChannelCount = other.uvChannelCount;
	normalChannelCount = other.normalChannelCount;
	tangentChannelCount = other.tangentChannelCount;
	binormalChannelCount = other.colorChannelCount;

	aabb1 = other.aabb1;
	polygons1 = other.polygons1;
}

Mesh& Mesh::operator = (const Mesh& other)
{
	maxMaterialIdx = other.maxMaterialIdx;

	colorChannelCount = other.colorChannelCount;
	uvChannelCount = other.uvChannelCount;
	normalChannelCount = other.normalChannelCount;
	tangentChannelCount = other.tangentChannelCount;
	binormalChannelCount = other.colorChannelCount;

	aabb1 = other.aabb1;
	polygons1 = other.polygons1;

	return *this;
}

void Mesh::Flip()
{
	for (size_t i = 0; i < polygons1.size(); i++)
	{
		polygons1[i].Flip();
	}
}

bool Mesh::IsEmpty() const
{
	return polygons1.size() == 0;
}

void Mesh::Transform(const Vector3& t, const Vector3& s)
{
	for (size_t i = 0; i < polygons1.size(); i++)
	{
		polygons1[i].Transform(t, s);
	}
}

void Mesh::Add(const Polygon& polygon)
{
	polygons1.push_back(polygon);
}

const AABB& Mesh::GetAABB() const
{
	return aabb1;
}

int Mesh::GetPolygonCount() const
{
	return polygons1.size();
}

const Polygon& Mesh::GetPolygon(int i) const
{
	return polygons1[i];
}

int Mesh::GetVerticesCount() const
{
	int verticesCount = 0;
	for (size_t i = 0; i < polygons1.size(); i++)
	{
		verticesCount += polygons1[i].GetVerticesCount();
	}

	return verticesCount;
}

void ConvertTriangleFanToTriangles(Mesh& mesh)
{
	std::vector<Polygon> oldPolygons = mesh.polygons1;
	mesh.polygons1.clear();

	for (size_t i = 0; i < oldPolygons.size(); i++)
		ConvertTriangleFanToTriangles(oldPolygons[i], mesh.polygons1);
}

Mesh Intersect(const Mesh& m0, const Mesh& m1)
{
#if 0
	var a = new CSG.Node(this.clone().polygons);
	var b = new CSG.Node(csg.clone().polygons);
	a.invert();
	b.clipTo(a);
	b.invert();
	a.clipTo(b);
	b.clipTo(a);
	a.build(b.allPolygons());
	a.invert();
	return CSG.fromPolygons(a.allPolygons());
#endif
	Mesh result;

	if (!m0.IsEmpty() && !m1.IsEmpty())
	{
		BSP a(m0.polygons1);
		BSP b(m1.polygons1);

		a.Invert();
		b.ClipBy(a);		// find polygon of b inside ~a
		b.Invert();
		a.ClipBy(b);		// find polygon of a inside ~b

		b.ClipBy(a);		// remove b polygons coplanar with a

		// join polygons in a, b
		// a.FromPolygons(b.ToPolygons());
		std::vector<Polygon> polygons;
		b.ToPolygons(polygons);
		a.FromPolygons(polygons);

		a.Invert();			// ~(~A | ~B)

		// output
		a.ToPolygons(result.polygons1);

		result.maxMaterialIdx = m0.maxMaterialIdx;
		result.colorChannelCount = m0.colorChannelCount;
		result.uvChannelCount = m0.uvChannelCount;
		result.normalChannelCount = m0.normalChannelCount;
		result.tangentChannelCount = m0.tangentChannelCount;
		result.binormalChannelCount = m0.binormalChannelCount;
		result.aabb1 = m0.aabb1;
	}
	else if (m0.IsEmpty() && !m1.IsEmpty())
	{
		result = Mesh();
	}
	else if (!m0.IsEmpty() && m1.IsEmpty())
	{
		result = Mesh();
	}
	else// if (m0.IsEmpty() && m1.IsEmpty())
	{
		result = Mesh();
	}

	return result;
}