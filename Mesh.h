#ifndef _Mesh_h_
#define _Mesh_h_

#include "AABB.h"
#include "Polygon.h"
#include "DataOptimizer.h"
#include "Edge.h"

////////////////////////////////////////////////////////////////////////
class Mesh
{
public:
	Mesh(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1);

	~Mesh();

	Mesh(const Mesh& other);

	Mesh& operator = (const Mesh& other);

	bool IsClosed() const;

	int GetMaxGroupIdx() const;

	int GetMaxMaterialIdx() const;

	int GetVerticesCount() const;
	
	int GetColorChannelCount() const;
	
	int GetUVChannelCount() const;
	
	int GetNormalChannelCount() const;
	
	int GetTangentChannelCount() const;

	int GetBinormalChannelCount() const;

	const AABB& GetAABB() const;

	size_t GetPolygonCount() const;	

	int GetPolygonMaterialIdx(size_t polyIdx) const;

	int GetPolygonEdgesCount(size_t polyIdx) const;

	const Edge& GetPolygonEdge(size_t polyIdx, size_t edgeIdx) const;

	const Vertex& GetPolygonVertex(size_t polyIdx, size_t vertexIdx) const;

	const Vector3& GetPolygonEdgeStartPosition(size_t polyIdx, size_t edgeIdx) const;

	const Vector3& GetPolygonEdgeEndPosition(size_t polyIdx, size_t edgeIdx) const;

	const int& GetPolygonEdgeAdjacentPolygonIdx(size_t polyIdx, size_t edgeIdx) const;

	const int& GetPolygonGroupID(size_t polyIdx) const;

	const Plane& GetPolygonPlane(size_t polyIdx) const;

	const AABB& GetPolygonAABB(size_t polyIdx) const;

	void SetPolygonEdgeAdjacentPolygonIdx(size_t polyIdx, size_t edgeIdx, size_t adjacentPolygonIdx);

	void SetPolygonGroupID(size_t polyIdx, int groupID);

	void Begin(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1);

	void BeginPolygon(int groupIdx_, int materialIdx_);

	void AddPolygonVertices(const std::vector<Vertex>& vertices);

	void AddPolygonVertex(const Vertex& vertex);

	void EndPolygon();

	void End();

	void Clear(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1);

	void Flip();

	bool IsEmpty() const;
private:
	void SortPolygonsByMaterialIdx();
	void ComputeAABB();
	bool ComputePolygonsAdjacency();
	bool ComputePolygonsAdjacency(std::function<void(int, int, int)> setAdjacentCB);
	bool ComputeClosed();
	void Slice(const Plane& plane);

	bool isClosed;

	int maxGroupIdx;
	int maxMaterialIdx;
	int verticesCount;

	int colorChannelCount;
	int uvChannelCount;
	int normalChannelCount;
	int tangentChannelCount;
	int binormalChannelCount;
	
	AABB aabb;

	std::vector<Polygon> polygons;
	DataOptimizer<Vertex> verticesOptimizer;
	DataOptimizer<Vector3> edgeVertexOptimizer;
};

typedef std::vector<Mesh> MeshArray;

#endif