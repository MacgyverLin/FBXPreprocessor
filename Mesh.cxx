#include "Mesh.h"
#include "BSP.h"

Mesh::Mesh(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
{
	Clear(colorChannelCount_, uvChannelCount_, normalChannelCount_, tangentChannelCount_, binormalChannelCount_);
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
	binormalChannelCount = other.binormalChannelCount;

	aabb = other.aabb;
	polygons = other.polygons;
}

Mesh& Mesh::operator = (const Mesh& other)
{
	maxMaterialIdx = other.maxMaterialIdx;

	colorChannelCount = other.colorChannelCount;
	uvChannelCount = other.uvChannelCount;
	normalChannelCount = other.normalChannelCount;
	tangentChannelCount = other.tangentChannelCount;
	binormalChannelCount = other.binormalChannelCount;

	aabb = other.aabb;
	polygons = other.polygons;

	return *this;
}

int Mesh::GetMaxMaterialIdx() const
{
	return maxMaterialIdx;
}

void Mesh::SetColorChannelCount(int colorChannelCount_)
{
	colorChannelCount = colorChannelCount_;
}

void Mesh::SetUVChannelCount(int uvChannelCount_)
{
	uvChannelCount = uvChannelCount_;
}

void Mesh::SetNormalChannelCount(int normalChannelCount_)
{
	normalChannelCount = normalChannelCount_;
}

void Mesh::SetTangentChannelCount(int tangentChannelCount_)
{
	tangentChannelCount = tangentChannelCount_;
}

void Mesh::SetBinormalChannelCount(int binormalChannelCount_)
{
	binormalChannelCount = binormalChannelCount_;
}

int Mesh::GetColorChannelCount() const
{
	return colorChannelCount;
}

int Mesh::GetUVChannelCount() const
{
	return uvChannelCount;
}

int Mesh::GetNormalChannelCount() const
{
	return normalChannelCount;
}

int Mesh::GetTangentChannelCount() const
{
	return tangentChannelCount;
}

int Mesh::GetBinormalChannelCount() const
{
	return binormalChannelCount;
}

void Mesh::SetAABB(const AABB& aabb_)
{
	aabb = aabb_;
}

const AABB& Mesh::GetAABB() const
{
	return aabb;
}

size_t Mesh::GetPolygonCount() const
{
	return polygons.size();
}

const Polygon& Mesh::GetPolygon(int i) const
{
	return polygons[i];
}

const std::vector<Polygon>& Mesh::GetPolygons() const
{
	return polygons;
}

std::vector<Polygon>& Mesh::GetPolygons()
{
	return polygons;
}

size_t Mesh::GetVerticesCount() const
{
	int verticesCount = 0;
	for (size_t i = 0; i < polygons.size(); i++)
	{
		verticesCount += polygons[i].GetVerticesCount();
	}

	return verticesCount;
}

void Mesh::Add(const Polygon& polygon)
{
	if (maxMaterialIdx < polygon.GetMaterialIdx())
		maxMaterialIdx = polygon.GetMaterialIdx();

	polygons.push_back(polygon);
}

void Mesh::Clear(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
{
	maxMaterialIdx = 0;

	colorChannelCount = colorChannelCount_;
	uvChannelCount = uvChannelCount_;
	normalChannelCount = normalChannelCount_;
	tangentChannelCount = tangentChannelCount_;
	binormalChannelCount = binormalChannelCount_;

	aabb = AABB();
	polygons.clear();
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

void FixMaterialOrder(Mesh& mesh)
{
	std::vector<Vertex> vertices(3);
	for (size_t i = 0; i < vertices.size(); i++)
	{
		vertices[i] =
			Vertex
			(
				Vector3(0.0f + Math::UnitRandom() * 0.01f, 0.0f + Math::UnitRandom() * 0.01f, 0.0f + Math::UnitRandom() * 0.01f),
				// Vector3(0.0f, 0.0f, 0.0f),
				Vector3(1.0f, 0.0f, 0.0f),
				Vector2(0.0f, 0.0f)
			);
	}

	std::vector<Polygon> oldPolygons = mesh.polygons;
	
	mesh.polygons.clear();

	for (size_t i = 0; i < mesh.GetMaxMaterialIdx() + 1; i++)
		mesh.Add(Polygon(i, vertices));

	mesh.polygons.insert(mesh.polygons.end(), oldPolygons.begin(), oldPolygons.end());
}

void Triangulate(Mesh& mesh)
{
	std::vector<Polygon> oldPolygons = mesh.polygons;
	
	mesh.polygons.clear();

	for (size_t i = 0; i < oldPolygons.size(); i++)
		Triangulate(oldPolygons[i], mesh.polygons);
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
		BSP a(m0.polygons);
		BSP b(m1.polygons);

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
		result.maxMaterialIdx = std::max(m0.maxMaterialIdx, m1.maxMaterialIdx);
		result.colorChannelCount = m0.colorChannelCount;
		result.uvChannelCount = m0.uvChannelCount;
		result.normalChannelCount = m0.normalChannelCount;
		result.tangentChannelCount = m0.tangentChannelCount;
		result.binormalChannelCount = m0.binormalChannelCount;
		result.aabb = m0.aabb;
		a.ToPolygons(result.polygons);
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