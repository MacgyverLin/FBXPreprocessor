#ifndef _MeshSeparator_h_
#define _MeshSeparator_h_

#include "Argument.h"
#include "Matrix4.h"
#include "Mesh.h"
#include "BSP.h"

class MeshSeparator
{
public:
	MeshSeparator();
	~MeshSeparator();
	
	/////////////////////////////////////////////////////////////////////////////////
	bool Process(const std::vector<Mesh>& srcMeshes_, std::vector<MeshArray>& resultMeshArrays_);
private:
	bool Process(int sceneMaxMaterialIdx_, const Mesh& srcMesh_, Mesh& resultMesh_);

#define USE_METHOD1
// #define USE_METHOD2

#if defined(USE_METHOD1)
	bool Slice(const Mesh& srcMesh_, BSP& bsp_, Mesh& resultMesh_);

	int ComputeSceneMaxMaterialIdx(const std::vector<Mesh>& srcMeshes_);
	int GetBSPRegion(const Mesh& srcMesh_, const std::vector<const BSP*>& bspArray_, const Vector3& p_);

	typedef std::multimap<int, int> UnorderedEdges;
	bool BuildCrossSectionLoops2(const Mesh& resultMesh_, std::vector<Loop2>& crossSectionLoops_);
	bool BuildCrossSectionEdges2(const Mesh& resultMesh_, std::map<int, UnorderedEdges>& unorderedEdgesGroups_);
	bool BuildCrossSectionLoopsFromEdges2(std::map<int, UnorderedEdges> unorderedEdgesGroups_, std::vector<Loop2>& crossSectionLoops_);
	bool PrintCrossSectionLoops2(const Mesh& resultMesh_, const std::vector<Loop2>& loops_);
	bool AddCrossSectionLoopsToMesh2(const std::vector<Loop2>& crossSectionLoops_, int sceneMaxMaterialIdx_, float textureSize, Mesh& resultMesh_);
	Vertex MakeVertex(const Mesh& mesh, const Matrix4& projMatrix, const Vector3& center, const Vector3& normal, const Vector3& tangent, const Vector3& binormal, float textureSize, int groupID);
#elif defined(USE_METHOD2)
	bool Group(const Mesh& srcMesh_, Mesh& resultMesh_, int count_);
#endif
};

#endif