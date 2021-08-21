#ifndef _Mesh_h_
#define _Mesh_h_

#include "AABB.h"
#include "Polygon.h"

class Mesh
{
public:
	Mesh(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1);

	~Mesh();

	Mesh(const Mesh& other);

	Mesh& operator = (const Mesh& other);

	int GetMaxMaterialIdx() const;

	void SetColorChannelCount(int colorChannelCount_);
	void SetUVChannelCount(int uvChannelCount_);
	void SetNormalChannelCount(int normalChannelCount_);
	void SetTangentChannelCount(int tangentChannelCount_);
	void SetBinormalChannelCount(int binormalChannelCount_);
	int GetColorChannelCount() const;
	int GetUVChannelCount() const;
	int GetNormalChannelCount() const;
	int GetTangentChannelCount() const;
	int GetBinormalChannelCount() const;

	void SetAABB(const AABB& aabb_);

	const AABB& GetAABB() const;

	size_t GetPolygonCount() const;

	const Polygon& GetPolygon(int i) const;

	const std::vector<Polygon>& GetPolygons() const;

	std::vector<Polygon>& GetPolygons();

	size_t GetVerticesCount() const;

	void Add(const Polygon& polygon);

	void Clear(int colorChannelCount_=0, int uvChannelCount_=1, int normalChannelCount_=1, int tangentChannelCount_=1, int binormalChannelCount_=1);

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