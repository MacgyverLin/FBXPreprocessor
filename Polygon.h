#ifndef _Polygon_h_
#define _Polygon_h_

#include "Vertex.h"

class Polygon
{
	friend class MeshBuilder;
	friend class MeshSlicer;
	friend class BSPMeshSlicer;
	friend class FBXMeshBuilder;
	friend class BSP;
public:
	Polygon();

	Polygon(int materialIdx, const std::vector<Vertex>& vertices);

	~Polygon();

	Polygon(const Polygon& other);

	Polygon& operator = (const Polygon& other);

	void Clear();

	void Flip();

	bool IsEmpty() const;

	void Transform(const Vector3& t, const Vector3& s);
private:
	int materialIdx;
	std::vector<Vertex> vertices;
};

#endif