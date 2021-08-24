#ifndef _Polygon_h_
#define _Polygon_h_

#include "Vertex.h"
#include "Plane.h"
#include "AABB.h"
#include "DataOptimizer.h"
#include "Edge.h"

class Adjacency
{
public:
	Adjacency(bool flipped_ = false, int edgeIdx_ = -1, int polygonIdx_ = -1)
	{
		flipped = flipped_;

		edgeIdx = edgeIdx_;
		
		polygonIdx = polygonIdx_;
	}

	~Adjacency()
	{
	}

	int& EdgeIndex()
	{
		return edgeIdx;
	}

	int& PolygonIdx()
	{
		return polygonIdx;
	}

	bool& EdgeFlipped()
	{
		return flipped;
	}

	const int& EdgeIndex() const
	{
		return edgeIdx;
	}

	const int& PolygonIdx() const
	{
		return polygonIdx;
	}

	const bool& EdgeFlipped() const
	{
		return flipped;
	}

	void FlipEdge()
	{
		flipped = !flipped;
	}

	bool flipped;
	int edgeIdx;
	int polygonIdx;
};

class Polygon
{
	friend class Mesh;
	friend class BSP;
public:
	Polygon(int groupIdx_ = 0, int materialIdx_ = 0);

	~Polygon();

	Polygon(const Polygon& other);

	Polygon& operator = (const Polygon& other);

	int GetMaterialIdx() const;

	int GetVerticesCount() const;

	const Vertex& GetVertex(int i) const;

	const std::vector<Vertex>& GetVertices() const;

	int GetGroupIdx() const;

	const Plane& GetPlane() const;

	const AABB& GetAABB() const;

	const Adjacency& GetAdjacency(int i) const;

	const std::vector<Adjacency>& GetAdjacencies() const;

	void Clear();

	void Flip();

	bool IsEmpty() const;
private:
	void Begin(int groupIdx_, int materialIdx_);

	void Add(const std::vector<Vertex>& vertices);

	void Add(const Vertex& vertex);

	void End();
	void End(DataOptimizer<Vector3>& positionOptimizer, DataOptimizer<Edge>& edgeOptimizer);

	int materialIdx;
	std::vector<Vertex> vertices;

	int groupIdx;
	Plane plane;
	AABB aabb;
	std::vector<Adjacency> adjacencies;
};

#endif