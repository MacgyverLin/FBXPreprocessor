#ifndef _Mesh_h_
#define _Mesh_h_

#include "AABB.h"
#include "Polygon.h"
#include "DataOptimizer.h"
#include "Edge.h"

////////////////////////////////////////////////////////////////////////
class Mesh
{
public:
	Mesh(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1);

	~Mesh();

	Mesh(const Mesh& other);

	Mesh& operator = (const Mesh& other);

	int GetMaxMaterialIdx() const;
	int GetColorChannelCount() const;
	int GetUVChannelCount() const;
	int GetNormalChannelCount() const;
	int GetTangentChannelCount() const;
	int GetBinormalChannelCount() const;

	const AABB& GetAABB() const;

	size_t GetPolygonCount() const;	
	
	const Polygon& GetPolygon(int i) const;

	const std::vector<Polygon>& GetPolygons() const;

	size_t GetVerticesCount() const;

	void Begin(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1);

	void BeginPolygon(int materialIdx);

	void BeginPolygon(const std::vector<Polygon>& polys);

	void BeginPolygon(const Polygon& polygon);

	void AddPolygonVertex(const std::vector<Vertex>& vertices);

	void AddPolygonVertex(const Vertex& vertex);

	void EndPolygon();

	void End();

	void Clear(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1);

	void Flip();

	bool IsEmpty() const;
private:
	int maxMaterialIdx;

	int colorChannelCount;
	int uvChannelCount;
	int normalChannelCount;
	int tangentChannelCount;
	int binormalChannelCount;
	AABB aabb;

	std::vector<Polygon> polygons;
	DataOptimizer<Vector3> positionOptimizer;
	DataOptimizer<Edge> edgeOptimizer;
};

extern bool FixMaterialOrder(Mesh& mesh);

extern void Triangulate(Mesh& mesh);

extern Mesh Intersect(const Mesh& m0, const Mesh& m1);

typedef std::vector<Mesh> MeshArray;


/*
class Mesh : public MeshBase
{
public:
	Mesh(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1);

	~Mesh();

	Mesh(const Mesh& other);

	Mesh& operator = (const Mesh& other);

	virtual size_t GetPolygonCount() const override;

	virtual const Polygon& GetPolygon(int i) const override;

	virtual const std::vector<Polygon>& GetPolygons() const override;

	virtual size_t GetVerticesCount() const override;

	virtual void Begin(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1) override;

	virtual void Add(const std::vector<Polygon>& polygons) override;

	virtual void Add(const Polygon& polygon) override;

	virtual void End() override;

	virtual void Clear(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1) override;

	virtual void Flip() override;

	virtual bool IsEmpty() const override;
private:
	std::vector<Polygon> polygons;
};

extern bool FixMaterialOrder(Mesh& mesh);

extern void Triangulate(Mesh& mesh);

extern Mesh Intersect(const Mesh& m0, const Mesh& m1);

typedef std::vector<Mesh> MeshArray;
*/

#endif