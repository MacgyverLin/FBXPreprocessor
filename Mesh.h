#ifndef _Mesh_h_
#define _Mesh_h_

#include "AABB.h"
#include "Polygon.h"

class Mesh
{
	friend class MeshBuilder;
	friend class MeshSlicer;
	friend class MeshSeparator;
	friend class BSPMeshSlicer;
	friend class FBXMeshBuilder;
	friend class BSP;
public:
	Mesh();

	~Mesh();

	Mesh(const Mesh& other);

	Mesh& operator = (const Mesh& other);

	const AABB& GetAABB() const;

	size_t GetPolygonCount() const;

	const Polygon& GetPolygon(int i) const;

	size_t GetVerticesCount() const;

	void Add(const Polygon& polygon);

	void Flip();

	bool IsEmpty() const;

	friend void FixMaterialOrder(Mesh& mesh);

	friend void Triangulate(Mesh& mesh);

	friend Mesh Intersect(const Mesh& m0, const Mesh& m1);
private:
	int maxMaterialIdx;

	int colorChannelCount;
	int uvChannelCount;
	int normalChannelCount;
	int tangentChannelCount;
	int binormalChannelCount;
	AABB aabb;
	std::vector<Polygon> polygons;
};

typedef std::vector<Mesh> MeshArray;

#endif