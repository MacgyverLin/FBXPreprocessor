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
	// polygons = other.polygons;
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
	// polygons = other.polygons;

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

void MeshBase::Add(const Polygon& polygon)
{
	if (maxMaterialIdx < polygon.GetMaterialIdx())
		maxMaterialIdx = polygon.GetMaterialIdx();
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








///////////////////////////
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

/*
const std::vector<Polygon>& Mesh::GetPolygons() const
{
	return polygons;
}

std::vector<Polygon>& Mesh::GetPolygons()
{
	return polygons;
}
*/

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
	MeshBase::Add(polygon);

	polygons.push_back(polygon);
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
	// Create GetMaxMaterialIdx() of dummy polygons for enforce material order
	std::vector<Polygon> oldPolygons = mesh.polygons;
	
	mesh.polygons.clear();
	for (size_t i = 0; i <= mesh.GetMaxMaterialIdx(); i++)
		mesh.Add(Polygon(i, vertices));

	mesh.polygons.insert(mesh.polygons.end(), oldPolygons.begin(), oldPolygons.end());

	return true;
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


//////////////////////////////////////////////////////////////////////////////
IndexMesh::IndexMesh(const Mesh& mesh)
	: MeshBase(mesh.GetColorChannelCount(), mesh.GetUVChannelCount(), 
		mesh.GetNormalChannelCount(), mesh.GetTangentChannelCount(), mesh.GetBinormalChannelCount())
{
}

IndexMesh::~IndexMesh()
{
}

IndexMesh::IndexMesh(const IndexMesh& other)
	: MeshBase(other)
{
	// polygons = other.polygons;
}

IndexMesh& IndexMesh::operator = (const IndexMesh& other)
{
	*((MeshBase*)this) = *((MeshBase*)&other);

	// polygons = other.polygons;

	return *this;
}

size_t IndexMesh::GetPolygonCount() const
{
	//return polygons.size();
	return 0;
}

const Polygon& IndexMesh::GetPolygon(int i) const
{
	//return polygons[i];
	return Polygon();
}

/*
const std::vector<Polygon>& IndexMesh::GetPolygons() const
{
	return polygons;
}

std::vector<Polygon>& IndexMesh::GetPolygons()
{
	return polygons;
}
*/

size_t IndexMesh::GetVerticesCount() const
{
	/*
	int verticesCount = 0;
	for (size_t i = 0; i < polygons.size(); i++)
	{
		verticesCount += polygons[i].GetVerticesCount();
	}

	return verticesCount;
	*/

	return 0;
}

void IndexMesh::Add(const Polygon& polygon)
{
	MeshBase::Add(polygon);

	// polygons.push_back(polygon);
}

void IndexMesh::Clear(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
{
	MeshBase::Clear(colorChannelCount_, uvChannelCount_, normalChannelCount_, tangentChannelCount_, binormalChannelCount_);

	// polygons.clear();
}

void IndexMesh::Flip()
{
	//for (size_t i = 0; i < polygons.size(); i++)
	//{
		// polygons[i].Flip();
	//}
}

bool IndexMesh::IsEmpty() const
{
	//return polygons.size() == 0;
	return 0;
}
