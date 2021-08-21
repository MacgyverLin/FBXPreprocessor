#include "Mesh.h"
#include "BSP.h"

MeshBase::MeshBase(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
{
	Clear(colorChannelCount_, uvChannelCount_, normalChannelCount_, tangentChannelCount_, binormalChannelCount_);
}

MeshBase::~MeshBase()
{
}

MeshBase::MeshBase(const MeshBase& other)
{
	maxMaterialIdx = other.maxMaterialIdx;

	colorChannelCount = other.colorChannelCount;
	uvChannelCount = other.uvChannelCount;
	normalChannelCount = other.normalChannelCount;
	tangentChannelCount = other.tangentChannelCount;
	binormalChannelCount = other.binormalChannelCount;

	aabb = other.aabb;
}

MeshBase& MeshBase::operator = (const MeshBase& other)
{
	maxMaterialIdx = other.maxMaterialIdx;

	colorChannelCount = other.colorChannelCount;
	uvChannelCount = other.uvChannelCount;
	normalChannelCount = other.normalChannelCount;
	tangentChannelCount = other.tangentChannelCount;
	binormalChannelCount = other.binormalChannelCount;

	aabb = other.aabb;

	return *this;
}

int MeshBase::GetMaxMaterialIdx() const
{
	return maxMaterialIdx;
}

void MeshBase::SetColorChannelCount(int colorChannelCount_)
{
	colorChannelCount = colorChannelCount_;
}

void MeshBase::SetUVChannelCount(int uvChannelCount_)
{
	uvChannelCount = uvChannelCount_;
}

void MeshBase::SetNormalChannelCount(int normalChannelCount_)
{
	normalChannelCount = normalChannelCount_;
}

void MeshBase::SetTangentChannelCount(int tangentChannelCount_)
{
	tangentChannelCount = tangentChannelCount_;
}

void MeshBase::SetBinormalChannelCount(int binormalChannelCount_)
{
	binormalChannelCount = binormalChannelCount_;
}

int MeshBase::GetColorChannelCount() const
{
	return colorChannelCount;
}

int MeshBase::GetUVChannelCount() const
{
	return uvChannelCount;
}

int MeshBase::GetNormalChannelCount() const
{
	return normalChannelCount;
}

int MeshBase::GetTangentChannelCount() const
{
	return tangentChannelCount;
}

int MeshBase::GetBinormalChannelCount() const
{
	return binormalChannelCount;
}

void MeshBase::SetAABB(const AABB& aabb_)
{
	aabb = aabb_;
}

const AABB& MeshBase::GetAABB() const
{
	return aabb;
}

void MeshBase::Begin(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
{
	maxMaterialIdx = 0;

	colorChannelCount = colorChannelCount_;
	uvChannelCount = uvChannelCount_;
	normalChannelCount = normalChannelCount_;
	tangentChannelCount = tangentChannelCount_;
	binormalChannelCount = binormalChannelCount_;

	aabb = AABB();
}

void MeshBase::Add20(const std::vector<Polygon>& polys)
{
	for(auto& polygon : polys)
	{
		if (maxMaterialIdx < polygon.GetMaterialIdx())
			maxMaterialIdx = polygon.GetMaterialIdx();
	}
}

void MeshBase::Add20(const Polygon& polygon)
{
	if (maxMaterialIdx < polygon.GetMaterialIdx())
		maxMaterialIdx = polygon.GetMaterialIdx();
}

void MeshBase::End()
{
	// update aabb
}

void MeshBase::Clear(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
{
	maxMaterialIdx = 0;

	colorChannelCount = colorChannelCount_;
	uvChannelCount = uvChannelCount_;
	normalChannelCount = normalChannelCount_;
	tangentChannelCount = tangentChannelCount_;
	binormalChannelCount = binormalChannelCount_;

	aabb = AABB();
}

//////////////////////////////////////////////////////
Mesh::Mesh(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
: MeshBase(colorChannelCount_, uvChannelCount_, normalChannelCount_, tangentChannelCount_, binormalChannelCount_)
{
}

Mesh::~Mesh()
{
}

Mesh::Mesh(const Mesh& other)
: MeshBase(other)
{
	polygons = other.polygons;
}

Mesh& Mesh::operator = (const Mesh& other)
{
	*((MeshBase*)this) = *((MeshBase*)&other);

	polygons = other.polygons;

	return *this;
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

size_t Mesh::GetVerticesCount() const
{
	int verticesCount = 0;
	for (size_t i = 0; i < polygons.size(); i++)
	{
		verticesCount += polygons[i].GetVerticesCount();
	}

	return verticesCount;
}

void Mesh::Begin(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
{
	MeshBase::Begin(colorChannelCount_, uvChannelCount_, normalChannelCount_, tangentChannelCount_, binormalChannelCount_);

	polygons.clear();
}

void Mesh::Add20(const std::vector<Polygon>& polys)
{
	MeshBase::Add20(polys);

	polygons.insert(polygons.end(), polys.begin(), polys.end());
}

void Mesh::Add20(const Polygon& polygon)
{
	MeshBase::Add20(polygon);

	polygons.push_back(polygon);
}

void Mesh::End()
{
	// update aabb

	MeshBase::End();
}

void Mesh::Clear(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
{
	MeshBase::Clear(colorChannelCount_, uvChannelCount_, normalChannelCount_, tangentChannelCount_, binormalChannelCount_);

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

bool FixMaterialOrder(Mesh& mesh)
{
	////////////////////////////////////////////////////////////////
	// check Polygon Vertex
	if (mesh.GetPolygonCount() < 1)
		return false;

	const Polygon& polygon = mesh.GetPolygon(0);
	if (polygon.GetVerticesCount() < 1)
		return false;

	////////////////////////////////////////////////////////////////
	// prepare vertex
	const Vertex& v = polygon.GetVertex(0);
	std::vector<Vertex> vertices(3);
	for (size_t i = 0; i < vertices.size(); i++)
	{
		vertices[i] = v;
		vertices[i].position += Vector3(0.0f + Math::UnitRandom() * 0.01f, 0.0f + Math::UnitRandom() * 0.01f, 0.0f + Math::UnitRandom() * 0.01f);
	}

	////////////////////////////////////////////////////////////////
	// Add GetMaxMaterialIdx() of dummy polygons at the beginning for enforce material order
	Mesh oldMesh = mesh;

	mesh.Begin();

	for (size_t i = 0; i <= oldMesh.GetMaxMaterialIdx(); i++)
		mesh.Add20(Polygon(i, vertices));

	// for (size_t i = 0; i < oldMesh.GetPolygonCount(); i++)
	// 	mesh.Add20(oldMesh.GetPolygon(i));
	mesh.Add20(oldMesh.GetPolygons());

	mesh.End();

	/*
	////////////////////////////////////////////////////////////////
	// check Polygon Vertex
	if (mesh.GetPolygonCount() < 1)
		return false;

	const Polygon& polygon = mesh.GetPolygon(0);
	if (polygon.GetVerticesCount() < 1)
		return false;

	////////////////////////////////////////////////////////////////
	// prepare vertex
	const Vertex& v = polygon.GetVertex(0);
	std::vector<Vertex> vertices(3);
	for (size_t i = 0; i < vertices.size(); i++)
	{
		vertices[i] = v;
		vertices[i].position += Vector3(0.0f + Math::UnitRandom() * 0.01f, 0.0f + Math::UnitRandom() * 0.01f, 0.0f + Math::UnitRandom() * 0.01f);
	}

	////////////////////////////////////////////////////////////////
	// Create GetMaxMaterialIdx() of dummy polygons for enforce material order
	std::vector<Polygon> oldPolygons = mesh.polygons;
	
	mesh.polygons.clear();

	for (size_t i = 0; i <= mesh.GetMaxMaterialIdx(); i++)
		mesh.Add(Polygon(i, vertices));

	mesh.polygons.insert(mesh.polygons.end(), oldPolygons.begin(), oldPolygons.end());
	*/

	return true;
}

void Triangulate(Mesh& mesh)
{
	Mesh oldMesh = mesh;

	mesh.Begin(oldMesh.GetColorChannelCount(), oldMesh.GetUVChannelCount(), 
				oldMesh.GetNormalChannelCount(), oldMesh.GetTangentChannelCount(), oldMesh.GetBinormalChannelCount());

	for (size_t i = 0; i < oldMesh.GetPolygonCount(); i++)
	{
		std::vector<Polygon> polygons;
		Triangulate(oldMesh.GetPolygon(i), polygons);

		mesh.Add20(polygons);
	}
	
	mesh.End();
	/*
	std::vector<Polygon> oldPolygons = mesh.polygons;
	
	mesh.polygons.clear();

	for (size_t i = 0; i < oldPolygons.size(); i++)
		Triangulate(oldPolygons[i], mesh.polygons);
	*/
}

Mesh Intersect(const Mesh& m0, const Mesh& m1)
{
	Mesh result;

	if (!m0.IsEmpty() && !m1.IsEmpty())
	{
		////////////////////////////////////////////
		// BSP
		BSP a(m0.GetPolygons());
		BSP b(m1.GetPolygons());

		a.Invert();
		b.ClipBy(a);		// find polygon of b inside ~a
		b.Invert();
		a.ClipBy(b);		// find polygon of a inside ~b

		b.ClipBy(a);		// remove b polygons coplanar with a

		// join polygons in a, b
		// a.FromPolygons(b.ToPolygons());
		std::vector<Polygon> joinPolygons;
		b.ToPolygons(joinPolygons);
		a.FromPolygons(joinPolygons);

		a.Invert();			// ~(~A | ~B)
		
		std::vector<Polygon> resultPolygons;
		a.ToPolygons(resultPolygons);

		////////////////////////////////////////////
		// output
		result.Begin(m0.GetColorChannelCount(), m0.GetUVChannelCount(),
					m0.GetNormalChannelCount(), m0.GetTangentChannelCount(), m0.GetBinormalChannelCount());
		
		// result.maxMaterialIdx = std::max(m0.maxMaterialIdx, m1.maxMaterialIdx);
		result.Add20(resultPolygons);
		
		result.SetAABB(m0.GetAABB());
		result.End();
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

//////////////////////////////////////////////////////////////////////////////

// comparison
int Edge::CompareArrays(const Edge& v) const
{
	return memcmp(indices, v.indices, 2 * sizeof(indices[0]));
}

bool Edge::operator== (const Edge& v) const
{
	return CompareArrays(v) == 0;
}

bool Edge::operator!= (const Edge& v) const
{
	return CompareArrays(v) != 0;
}

bool Edge::operator<  (const Edge& v) const
{
	return CompareArrays(v) < 0;
}

bool Edge::operator<= (const Edge& v) const
{
	return CompareArrays(v) <= 0;
}

bool Edge::operator>  (const Edge& v) const
{
	return CompareArrays(v) > 0;
}

bool Edge::operator>= (const Edge& v) const
{
	return CompareArrays(v) >= 0;
}