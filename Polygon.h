#ifndef _Polygon_h_
#define _Polygon_h_

#include "Vertex.h"
#include "Plane.h"
#include "AABB.h"

class FlipEdge
{
public:
	FlipEdge(bool flip_, int edge_)
		: flip(flip_)
		, edge(edge_)
	{
	}

	bool flip;
	int edge;
};

class Polygon
{
	friend class Mesh;
	friend class BSP;
public:
	Polygon(int materialIdx = 0);

	~Polygon();

	Polygon(const Polygon& other);

	Polygon& operator = (const Polygon& other);

	int GetMaterialIdx() const;

	const Plane& GetPlane() const;

	const AABB& GetAABB() const;
	
	int GetVerticesCount() const;

	const Vertex& GetVertex(int i) const;

	const std::vector<Vertex>& GetVertices() const;

	int GetFlipEdgesCount() const;

	const FlipEdge& GetFlipEdge(int i) const;

	const std::vector<FlipEdge>& GetFlipEdges() const;

	void Clear();

	void Flip();

	bool IsEmpty() const;
private:
	void Begin(int materialIdx);

	void Add(const std::vector<Vertex>& vertices);

	void Add(const Vertex& vertex);

	void Add(const FlipEdge& edge);

	void End();

	int materialIdx;
	Plane plane;
	AABB aabb;
	std::vector<Vertex> vertices;
	std::vector<FlipEdge> flipEdges;
};

extern void Triangulate(const Polygon& polygon, std::vector<Polygon>& polygons);

#endif