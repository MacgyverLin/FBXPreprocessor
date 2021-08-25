#ifndef _Polygon_h_
#define _Polygon_h_

#include "Vertex.h"
#include "Plane.h"
#include "AABB.h"
#include "DataOptimizer.h"
#include "Edge.h"

class Polygon
{
	friend class Mesh;
public:
	Polygon(int groupIdx_ = 0, int materialIdx_ = 0);

	~Polygon();

	Polygon(const Polygon& other);

	Polygon& operator = (const Polygon& other);

	int GetMaterialIdx() const;

	int GetEdgesCount() const;

	const Edge& GetEdge(int i) const;

	const std::vector<Edge>& GetEdges() const;

	int GetGroupID() const;

	const Plane& GetPlane() const;

	const AABB& GetAABB() const;

	void SetGroupID(int groupID_);

	void Clear();

	void Flip();

	bool IsEmpty() const;
private:
	void Begin(int groupIdx_, int materialIdx_);

	void Add(int vertexIdx, int edgeIdx, int adjacentPolygonIdx);

	void End(DataOptimizer<Vertex>& verticesOptimizer_, DataOptimizer<Vector3>& edgeVertexOptimizer_);

	void SetEdgeAdjacentPolygonIdx(int edgeIdx, int adjacentPolygonIdx);

	int materialIdx;
	std::vector<Edge> edges;

	int groupID;
	Plane plane;
	AABB aabb;
};

#endif