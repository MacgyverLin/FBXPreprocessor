#include "Mesh.h"
#include "Common.h"
#include <map>
#include <algorithm>
//////////////////////////////////////////////////////
Mesh::Mesh(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
	: polygons()
	, verticesOptimizer()
	, edgeVertexOptimizer()
{
	Clear(colorChannelCount_, uvChannelCount_, normalChannelCount_, tangentChannelCount_, binormalChannelCount_);
}

Mesh::~Mesh()
{
}

Mesh::Mesh(const Mesh& other)
{
	isClosed = other.isClosed;

	maxGroupIdx = other.maxMaterialIdx;
	maxMaterialIdx = other.maxMaterialIdx;
	verticesCount = other.verticesCount;

	colorChannelCount = other.colorChannelCount;
	uvChannelCount = other.uvChannelCount;
	normalChannelCount = other.normalChannelCount;
	tangentChannelCount = other.tangentChannelCount;
	binormalChannelCount = other.binormalChannelCount;

	aabb = other.aabb;

	polygons = other.polygons;
	verticesOptimizer = other.verticesOptimizer;
	edgeVertexOptimizer = other.edgeVertexOptimizer;
}

Mesh& Mesh::operator = (const Mesh& other)
{
	isClosed = other.isClosed;

	maxGroupIdx = other.maxMaterialIdx;
	maxMaterialIdx = other.maxMaterialIdx;
	verticesCount = other.verticesCount;

	colorChannelCount = other.colorChannelCount;
	uvChannelCount = other.uvChannelCount;
	normalChannelCount = other.normalChannelCount;
	tangentChannelCount = other.tangentChannelCount;
	binormalChannelCount = other.binormalChannelCount;

	aabb = other.aabb;

	polygons = other.polygons;
	verticesOptimizer = other.verticesOptimizer;
	edgeVertexOptimizer = other.edgeVertexOptimizer;

	return *this;
}

bool Mesh::IsClosed() const
{
	return isClosed;
}

int Mesh::GetMaxGroupIdx() const
{
	return maxGroupIdx;
}

int Mesh::GetMaxMaterialIdx() const
{
	return maxMaterialIdx;
}

int Mesh::GetVerticesCount() const
{
	return verticesCount;
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

const AABB& Mesh::GetAABB() const
{
	return aabb;
}

size_t Mesh::GetPolygonCount() const
{
	return polygons.size();
}

int Mesh::GetPolygonMaterialIdx(size_t polyIdx) const
{
	return polygons[polyIdx].GetMaterialIdx();
}

int Mesh::GetPolygonEdgesCount(size_t polyIdx) const
{
	return polygons[polyIdx].GetEdgesCount();
}

const Edge& Mesh::GetPolygonEdge(size_t polyIdx, size_t edgeIdx) const
{
	return polygons[polyIdx].GetEdge(edgeIdx);
}

const Vertex& Mesh::GetPolygonVertex(size_t polyIdx, size_t vertexIdx) const
{
	const Edge& edge = GetPolygonEdge(polyIdx, vertexIdx);

	return verticesOptimizer.GetData(edge.GetVertexIdx());
}

const Vector3& Mesh::GetPolygonEdgeStartPosition(size_t polyIdx, size_t edgeIdx) const
{
	const Edge& edge = GetPolygonEdge(polyIdx, edgeIdx);

	return edgeVertexOptimizer.GetData(edge.GetStartIdx());
}

const Vector3& Mesh::GetPolygonEdgeEndPosition(size_t polyIdx, size_t edgeIdx) const
{
	const Edge& edge = GetPolygonEdge(polyIdx, edgeIdx);

	return edgeVertexOptimizer.GetData(edge.GetEndIdx());
}

int Mesh::GetPolygonEdgeAdjacentPolygonIdx(size_t polyIdx, size_t edgeIdx) const
{
	const Edge& edge = GetPolygonEdge(polyIdx, edgeIdx);

	return edge.GetAdjacentPolygonIdx();
}

int Mesh::GetPolygonGroupID(size_t polyIdx) const
{
	return polygons[polyIdx].GetGroupID();
}

const Plane& Mesh::GetPolygonPlane(size_t polyIdx) const
{
	return polygons[polyIdx].GetPlane();
}

const Vector3& Mesh::GetPolygonCenter(size_t polyIdx) const
{
	return polygons[polyIdx].GetCenter();
}

const AABB& Mesh::GetPolygonAABB(size_t polyIdx) const
{
	return polygons[polyIdx].GetAABB();
}

void Mesh::SetPolygonEdgeAdjacentPolygonIdx(size_t polyIdx, size_t edgeIdx, size_t adjacentPolygonIdx)
{
	return polygons[polyIdx].SetEdgeAdjacentPolygonIdx(edgeIdx, adjacentPolygonIdx);
}

void Mesh::SetPolygonGroupID(size_t polyIdx, int groupID)
{
	if (maxGroupIdx < groupID)
		maxGroupIdx = groupID;

	return polygons[polyIdx].SetGroupID(groupID);
}

void Mesh::BeginAppend()
{
}

void Mesh::Begin(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
{
	isClosed = false;

	maxGroupIdx = 0;
	maxMaterialIdx = 0;
	verticesCount = 0;

	colorChannelCount = colorChannelCount_;
	uvChannelCount = uvChannelCount_;
	normalChannelCount = normalChannelCount_;
	tangentChannelCount = tangentChannelCount_;
	binormalChannelCount = binormalChannelCount_;

	aabb = AABB();

	polygons.clear();
	verticesOptimizer.Clear();
	edgeVertexOptimizer.Clear();
}

void Mesh::BeginPolygon(int groupIdx_, int materialIdx_)
{
	if (maxGroupIdx < groupIdx_)
		maxGroupIdx = groupIdx_;

	if (maxMaterialIdx < materialIdx_)
		maxMaterialIdx = materialIdx_;

	polygons.push_back(Polygon());
	polygons.back().Begin(groupIdx_, materialIdx_);
}

void Mesh::AddPolygonVertices(const std::vector<Vertex>& vertices)
{
	for (auto& v : vertices)
	{
		AddPolygonVertex(v);
	}
}

void Mesh::AddPolygonVertex(const Vertex& vertex)
{
	Polygon& polygon = polygons.back();

	int idx0 = verticesOptimizer.Add(vertex);
	int idx1 = edgeVertexOptimizer.Add(vertex.position);
	idx0 = verticesOptimizer.Add(vertex);
	idx1 = edgeVertexOptimizer.Add(vertex.position);

	polygon.Add
	(
		idx0,
		idx1,
		-1
	);

	verticesCount++;
}

void Mesh::EndPolygon()
{
	Polygon& polygon = polygons.back();

	polygon.End(verticesOptimizer, edgeVertexOptimizer);
}

void Mesh::End()
{
	SortPolygonsByMaterialIdx();

	ComputeAABB();

	isClosed = ComputePolygonsAdjacency();
}

const Vertex& Mesh::GetVertex(int idx) const
{
	return verticesOptimizer.GetData(idx);
}

const Vector3& Mesh::GetEdgeStartVertex(int idx) const
{
	return edgeVertexOptimizer.GetData(idx);
}

int Mesh::GetMaxVertexIndex() const
{
	return verticesOptimizer.GetMaxIndex();
}

int Mesh::GetMaxEdgeIndex() const
{
	return edgeVertexOptimizer.GetMaxIndex();
}

void Mesh::SortPolygonsByMaterialIdx()
{
	std::sort(polygons.begin(), polygons.end(), [&](const Polygon& p0, const Polygon& p1) -> bool { return p0.GetMaterialIdx() < p1.GetMaterialIdx();  });
}

void Mesh::ComputeAABB()
{
	if (polygons.size() == 0)
	{
		aabb = AABB(Vector3::Zero, Vector3::Zero);
	}
	else
	{
		aabb = AABB(polygons[0].GetAABB());
		for (size_t i = 0; i < polygons.size(); i++)
			aabb += polygons[i].GetAABB();
	}
}

#include <experimental/map>
bool Mesh::ComputeClosed()
{
	std::map<int, int> isolatedEdges;

	DataOptimizer<Edge> edgeOptimizer;
	for (size_t polyIdx = 0; polyIdx < GetPolygonCount(); polyIdx++)
	{
		for (size_t edgeIdx = 0; edgeIdx < GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			Edge edge = GetPolygonEdge(polyIdx, edgeIdx);
			bool normalized = edge.IsNormalized();
			if (!normalized)
				edge.Flip();

			int edgeId = edgeOptimizer.Add(edge);
			if (normalized)
				isolatedEdges[edgeId] += 1;
			else
				isolatedEdges[edgeId] -= 1;
		}
	}

	std::experimental::erase_if(isolatedEdges, [](const auto& item) { return item.second == 0; });

	if (isolatedEdges.size() == 0)
		Debug::Verbose("Mesh is closed\n");

	return isolatedEdges.size() == 0;
}

bool Mesh::ComputePolygonsAdjacency()
{
	return ComputePolygonsAdjacency(std::bind(&Mesh::SetPolygonEdgeAdjacentPolygonIdx, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

bool Mesh::ComputePolygonsAdjacency(std::function<void(int, int, int)> setAdjacentCB)
{
	bool meshClosed = true;

	std::map<int, int> postiiveSidePolygons;
	std::map<int, int> negativeSidePolygons;
	DataOptimizer<Edge> edgeOptimizer;
	for (size_t polyIdx = 0; polyIdx < GetPolygonCount(); polyIdx++)
	{
		for (size_t edgeIdx = 0; edgeIdx < GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			Edge edge = GetPolygonEdge(polyIdx, edgeIdx);
			bool normalized = edge.IsNormalized();
			if (!normalized)
				edge.Flip();

			int edgeId = edgeOptimizer.Add(edge);
			if (normalized)
				postiiveSidePolygons[edgeId] = polyIdx;
			else
				negativeSidePolygons[edgeId] = polyIdx;
		}
	}


	for (size_t polyIdx = 0; polyIdx < GetPolygonCount(); polyIdx++)
	{
		for (size_t edgeIdx = 0; edgeIdx < GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			Edge edge = GetPolygonEdge(polyIdx, edgeIdx);
			bool normalized = edge.IsNormalized();
			if (!normalized)
				edge.Flip();

			int edgeId = edgeOptimizer.GetIndex(edge);
			if (normalized)
			{
				std::map<int, int>::iterator itr = negativeSidePolygons.find(edgeId);
				if (itr != negativeSidePolygons.end())
				{
					setAdjacentCB(polyIdx, edgeIdx, itr->second);
					// SetPolygonEdgeAdjacentPolygonIdx(polyIdx, edgeIdx, itr->second);
				}
			}
			else
			{
				std::map<int, int>::iterator itr = postiiveSidePolygons.find(edgeId);
				if (itr != postiiveSidePolygons.end())
				{
					setAdjacentCB(polyIdx, edgeIdx, itr->second);
					// SetPolygonEdgeAdjacentPolygonIdx(polyIdx, edgeIdx, itr->second);
				}
			}

			// Debug::Info("%d\n", GetPolygonEdge(polyIdx, edgeIdx).GetAdjacentPolygonIdx());

			if (GetPolygonEdge(polyIdx, edgeIdx).GetAdjacentPolygonIdx() == -1)
			{
				meshClosed = false;
			}
		}
	}

	if (meshClosed)
		Debug::Verbose("Mesh is closed\n");

	return meshClosed;
}

void Mesh::Clear(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
{
	isClosed = false;

	maxGroupIdx = 0;
	maxMaterialIdx = 0;
	verticesCount = 0;

	colorChannelCount = colorChannelCount_;
	uvChannelCount = uvChannelCount_;
	normalChannelCount = normalChannelCount_;
	tangentChannelCount = tangentChannelCount_;
	binormalChannelCount = binormalChannelCount_;
	aabb = AABB(Vector3::Zero, Vector3::Zero);

	polygons.clear();
	verticesOptimizer.Clear();
	edgeVertexOptimizer.Clear();
}

void Mesh::Flip()
{
	for (size_t i = 0; i < polygons.size(); i++)
		polygons[i].Flip();
}

bool Mesh::IsEmpty() const
{
	return polygons.size() == 0;
}