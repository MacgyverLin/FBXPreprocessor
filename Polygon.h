#ifndef _Polygon_h_
#define _Polygon_h_

#include "Vertex.h"
#include "Plane.h"
#include "AABB.h"

class DirectionEdge
{
public:
	DirectionEdge(bool flip_, int edgeIdx_)
		: flip(flip_)
		, edgeIdx(edgeIdx_)
	{
	}

	bool flip;
	int edgeIdx;
};

class Polygon
{
	friend class Mesh;
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

	int GetDirectionEdgesCount() const;

	const DirectionEdge& GetDirectionEdge(int i) const;

	const std::vector<DirectionEdge>& GetDirectionEdges() const;

	void Begin(int materialIdx);

	void Add(const std::vector<Vertex>& vertices);

	void Add(const Vertex& vertex);

	void End();

	void Clear();

	void Flip();

	bool IsEmpty() const;
private:
	void Add(const DirectionEdge& edge);

	int materialIdx;
	Plane plane;
	AABB aabb;
	std::vector<Vertex> vertices;
	std::vector<DirectionEdge> directionEdges;
};

extern void Triangulate(const Polygon& polygon, std::vector<Polygon>& polygons);

#endif