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

const AABB& MeshBase::GetAABB() const
{
	return aabb;
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

//////////////////////////////////////////////////////
Mesh::Mesh(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
	: MeshBase(colorChannelCount_, uvChannelCount_, normalChannelCount_, tangentChannelCount_, binormalChannelCount_)
	, positionOptimizer()
	, edgeOptimizer()
	, polygons()
{
}

Mesh::~Mesh()
{
}

Mesh::Mesh(const Mesh& other)
	: MeshBase(other)
{
	positionOptimizer = other.positionOptimizer;
	edgeOptimizer = other.edgeOptimizer;
	polygons = other.polygons;
}

Mesh& Mesh::operator = (const Mesh& other)
{
	*((MeshBase*)this) = *((MeshBase*)&other);

	positionOptimizer = other.positionOptimizer;
	edgeOptimizer = other.edgeOptimizer;
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
	maxMaterialIdx = 0;

	colorChannelCount = colorChannelCount_;
	uvChannelCount = uvChannelCount_;
	normalChannelCount = normalChannelCount_;
	tangentChannelCount = tangentChannelCount_;
	binormalChannelCount = binormalChannelCount_;

	aabb = AABB();


	positionOptimizer.Clear();
	edgeOptimizer.Clear();
	polygons.clear();
}

void Mesh::BeginPolygon(int materialIdx_)
{
	if (maxMaterialIdx < materialIdx_)
		maxMaterialIdx = materialIdx_;

	polygons.push_back(Polygon());
	polygons.back().Begin(materialIdx_);
}

void Mesh::BeginPolygon(const std::vector<Polygon>& polys)
{
	for (size_t i = 0; i < polys.size(); i++)
	{
		BeginPolygon(polys[i]);
		if (i != polys.size() - 1)
			EndPolygon();
	}
}

void Mesh::BeginPolygon(const Polygon& polygon)
{
	BeginPolygon(polygon.GetMaterialIdx());

	AddPolygonVertex(polygon.GetVertices());
}

void Mesh::AddPolygonVertex(const std::vector<Vertex>& vertices)
{
	for (auto& v : vertices)
	{
		AddPolygonVertex(v);
	}
}

void Mesh::AddPolygonVertex(const Vertex& vertex)
{
	Polygon& polygon = polygons.back();

	polygon.Add(vertex);
}

void Mesh::EndPolygon()
{
	Polygon& polygon = polygons.back();

	for (size_t i = 0; i < polygon.GetVerticesCount(); i++)
	{
		int i0 = (i + 0) % polygon.GetVerticesCount();
		int i1 = (i + 1) % polygon.GetVerticesCount();

		int vIdx0 = positionOptimizer.Add(polygon.GetVertex(i0).position);
		int vIdx1 = positionOptimizer.Add(polygon.GetVertex(i1).position);
		bool flip = (vIdx1 < vIdx0);
		int edgeIdx = edgeOptimizer.Add(Edge(vIdx0, vIdx1));
		
		polygon.Add(DirectionEdge(flip, edgeIdx));
	}

	polygon.End();
}

void Mesh::End()
{
	std::map<int, int> counters;
	for (size_t i = 0; i < polygons.size(); i++)
	{
		for (auto& edge : polygons[i].GetDirectionEdges())
		{
			counters[edge.edgeIdx] += (edge.flip ? -1 : 1);
		}
	}

	std::vector<int> isolatedEdges;
	for (auto& c : counters)
	{
		if (c.second != 0)
			isolatedEdges.push_back(c.first);
	}

	// update aabb
	for (size_t i = 0; i < polygons.size(); i++)
	{
		if(i == 0)
			aabb = polygons[i].GetAABB();
		else
			aabb += polygons[i].GetAABB();
	}
}

void Mesh::Clear(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
{
	MeshBase::Clear(colorChannelCount_, uvChannelCount_, normalChannelCount_, tangentChannelCount_, binormalChannelCount_);

	positionOptimizer.Clear();
	edgeOptimizer.Clear();
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

	//mesh.Begin(oldMesh.GetColorChannelCount(), oldMesh.GetUVChannelCount(), oldMesh.GetNormalChannelCount(), oldMesh.GetTangentChannelCount(), oldMesh.GetBinormalChannelCount());
	mesh.Begin();
	{
		for (size_t matIdx = 0; matIdx < oldMesh.GetMaxMaterialIdx() + 1; matIdx++)
		{
			mesh.BeginPolygon(matIdx);
			{
				mesh.AddPolygonVertex(vertices);
			}
			mesh.EndPolygon();
		}

		{
			mesh.BeginPolygon(oldMesh.GetPolygons());
			mesh.EndPolygon();
		}
	}
	mesh.End();

	return true;
}

void Triangulate(Mesh& mesh)
{
	Mesh oldMesh = mesh;

	mesh.Begin(oldMesh.GetColorChannelCount(), oldMesh.GetUVChannelCount(), oldMesh.GetNormalChannelCount(), oldMesh.GetTangentChannelCount(), oldMesh.GetBinormalChannelCount());
	{
		for (size_t i = 0; i < oldMesh.GetPolygonCount(); i++)
		{
			std::vector<Polygon> polygons;
			Triangulate(oldMesh.GetPolygon(i), polygons);

			mesh.BeginPolygon(polygons);
			mesh.EndPolygon();
		}
	}
	mesh.End();
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
		result.BeginPolygon(resultPolygons);
		result.EndPolygon();

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

/*
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

void Mesh::Add(const std::vector<Polygon>& polys)
{
	MeshBase::Add(polys);

	polygons.insert(polygons.end(), polys.begin(), polys.end());
}

void Mesh::Add(const Polygon& polygon)
{
	MeshBase::Add(polygon);

	polygons.push_back(polygon);
}

void Mesh::End()
{
	// update aabb
	for (size_t i = 0; i < polygons.size(); i++)
	{
		if (i == 0)
			aabb = polygons[i].GetAABB();
		else
			aabb += polygons[i].GetAABB();
	}

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

	for (size_t i = 0; i < oldMesh.GetMaxMaterialIdx() + 1; i++)
	{
		Polygon p;
		p.Begin(i);
		p.Add(vertices);
		p.End();

		mesh.Add(p);
	}
	mesh.Add(oldMesh.GetPolygons());

	mesh.End();

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

		mesh.Add(polygons);
	}

	mesh.End();
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
		result.Add(resultPolygons);

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
*/