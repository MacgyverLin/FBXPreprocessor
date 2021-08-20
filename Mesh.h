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

	void Add(const Polygon& polygon);

	const AABB& GetAABB() const;

	int GetPolygonCount() const;
	
	const Polygon& GetPolygon(int i) const;

	int GetVerticesCount() const;

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