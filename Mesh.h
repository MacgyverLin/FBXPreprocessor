#ifndef _Mesh_h_
#define _Mesh_h_

#include "AABB.h"
#include "Polygon.h"

////////////////////////////////////////////////////////////////////////
class MeshBase
{
public:
	MeshBase(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1);

	~MeshBase();

	MeshBase(const MeshBase& other);

	MeshBase& operator = (const MeshBase& other);

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

	virtual void Clear(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1);

	virtual void Flip() = 0;

	virtual bool IsEmpty() const = 0;
protected:
	int maxMaterialIdx;

	int colorChannelCount;
	int uvChannelCount;
	int normalChannelCount;
	int tangentChannelCount;
	int binormalChannelCount;
	AABB aabb;
};

////////////////////////////////////////////////////////////////////////
#include "DataOptimizer.h"

class Edge
{
public:
	Edge(int i0 = -1, int i1 = -1)
	{
		indices[0] = i0;
		indices[1] = i1;
	}

	~Edge()
	{
	}

	// comparison
	int CompareArrays(const Edge& v) const;

	bool operator== (const Edge& v) const;

	bool operator!= (const Edge& v) const;

	bool operator<  (const Edge& v) const;

	bool operator<= (const Edge& v) const;

	bool operator>  (const Edge& v) const;

	bool operator>= (const Edge& v) const;
private:
	int indices[2];
};

class Mesh : public MeshBase
{
public:
	Mesh(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1);

	~Mesh();

	Mesh(const Mesh& other);

	Mesh& operator = (const Mesh& other);

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

	virtual void Clear(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1) override;

	virtual void Flip() override;

	virtual bool IsEmpty() const override;
private:
	DataOptimizer<Vector3> positionOptimizer;
	DataOptimizer<Edge> edgeOptimizer;

	std::vector<Polygon> polygons;
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