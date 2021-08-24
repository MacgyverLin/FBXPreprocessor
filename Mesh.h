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

	bool IsClosed() const;

	int GetMaxGroupIdx() const;

	int GetMaxMaterialIdx() const;

	int GetVerticesCount() const;
	
	int GetColorChannelCount() const;
	
	int GetUVChannelCount() const;
	
	int GetNormalChannelCount() const;
	
	int GetTangentChannelCount() const;

	int GetBinormalChannelCount() const;

	const AABB& GetAABB() const;

	size_t GetPolygonCount() const;	
	
	const Polygon& GetPolygon(int i) const;

	const std::vector<Polygon>& GetPolygons() const;

	void Begin(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1);

	void BeginPolygon(int groupIdx_, int materialIdx_);

	void BeginPolygons(const std::vector<Polygon>& polys);

	void BeginPolygon(const Polygon& polygon);

	void AddPolygonVertices(const std::vector<Vertex>& vertices);

	void AddPolygonVertex(const Vertex& vertex);

	void EndPolygon();

	void End();

	void Clear(int colorChannelCount_ = 0, int uvChannelCount_ = 1, int normalChannelCount_ = 1, int tangentChannelCount_ = 1, int binormalChannelCount_ = 1);

	void Flip();

	bool IsEmpty() const;
private:
	void SortPolygonsByMaterialIdx();
	void UpdateAABB();
	bool UpdatePolygonsAdjacency();
	void UpdateIsolatedEdges();
	void Test();

	bool isClosed;

	int maxGroupIdx;
	int maxMaterialIdx;
	int verticesCount;

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

// extern void Triangulate(Mesh& mesh);

extern Mesh Intersect(const Mesh& m0, const Mesh& m1);

typedef std::vector<Mesh> MeshArray;

#endif