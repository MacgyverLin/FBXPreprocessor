#ifndef _Mesh_h_
#define _Mesh_h_

#include "AABB.h"
#include "Polygon.h"

class Mesh
{
	friend class MeshBuilder;
	friend class MeshSlicer;
	friend class BSPMeshSlicer;
	friend class FBXMeshBuilder;
	friend class BSP;
public:
	Mesh();

	~Mesh();

	Mesh(const Mesh& other);

	Mesh& operator = (const Mesh& other);

	void Flip();

	bool IsEmpty() const;

	void Transform(const Vector3& t, const Vector3& s);

	void Add(const Polygon& polygon);
private:
	AABB aabb;
	std::vector<Polygon> polygons;
};

typedef std::vector<Mesh> MeshArray;

#endif