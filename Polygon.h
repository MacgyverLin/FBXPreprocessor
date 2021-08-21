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

	// const std::vector<Vertex>& GetVertices() const;

	// std::vector<Vertex>& GetVertices();

	void Add(const Vertex& vertex);

	void Clear();

	void Flip();

	bool IsEmpty() const;

	Plane GetPlane() const;

	friend void Triangulate(const Polygon& polygon, std::vector<Polygon>& polygons);
private:
	int materialIdx;
	std::vector<Vertex> vertices;
};

#endif