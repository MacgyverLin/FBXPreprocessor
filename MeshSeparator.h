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
	bool Process(const std::vector<Mesh>& srcMeshes_, BSP* root_, std::vector<MeshArray>& resultMeshArrays_);
private:
	bool Process(const Mesh& srcMesh_, BSP* root_, Mesh& resultMesh_);
	bool Slice(const Mesh& srcMesh_, BSP* root_, Mesh& resultMesh_);
	int GetGroupIdx(const Mesh& srcMesh_, const Plane& splitPlane_, BSP* root_, const Vector3& p_);
	bool BuildCrossSectionLoops(const Mesh& resultMesh_, std::vector<Loop>& crossSectionLoops_);
	bool BuildCrossSectionEdges(const Mesh& resultMesh_, int groupID_, std::multimap<int, Edge>& isolatedEdges_);
	bool BuildCrossSectionLoopsFromEdges(const std::multimap<int, Edge> isolatedEdges_, int groupID_, std::vector<Loop>& isolatedLoops_);
	bool AddCrossSectionLoopsToMesh(const std::vector<Loop>& crossSectionLoops_, Mesh& resultMesh_);
	bool PrintCrossSectionLoops(const std::vector<Loop>& loops_);
};

#endif