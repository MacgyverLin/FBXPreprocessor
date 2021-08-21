#ifndef _Polygon_h_
#define _Polygon_h_

#include "Vertex.h"
#include "Plane.h"

class Polygon
{
public:
	Polygon(int materialIdx = 0);

	Polygon(int materialIdx, const std::vector<Vertex>& vertices);

	~Polygon();

	Polygon(const Polygon& other);

	Polygon& operator = (const Polygon& other);

	int GetMaterialIdx() const;
	
	int GetVerticesCount() const;

	const Vertex& GetVertex(int i) const;

	void Begin(int materialIdx = 0);

	void Add10(const std::vector<Vertex>& vertices);

	void Add10(const Vertex& vertex);

	void End();

	void Clear();

	void Flip();

	bool IsEmpty() const;

	Plane GetPlane() const;
private:
	int materialIdx;
	std::vector<Vertex> vertices;
};

extern void Triangulate(const Polygon& polygon, std::vector<Polygon>& polygons);

#endif