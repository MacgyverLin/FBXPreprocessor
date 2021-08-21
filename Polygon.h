#ifndef _Polygon_h_
#define _Polygon_h_

#include "Vertex.h"
#include "Plane.h"
#include "AABB.h"

class Polygon
{
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

	void Begin(int materialIdx);

	void Add(const std::vector<Vertex>& vertices);

	void Add(const Vertex& vertex);

	void End();

	void Clear();

	void Flip();

	bool IsEmpty() const;
private:
	int materialIdx;
	Plane plane;
	AABB aabb;
	std::vector<Vertex> vertices;
};

extern void Triangulate(const Polygon& polygon, std::vector<Polygon>& polygons);

#endif