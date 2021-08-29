#ifndef _MeshSeparator_h_
#define _MeshSeparator_h_

#include "Argument.h"
#include "Matrix4.h"
#include "Mesh.h"
#include "BSP.h"

typedef std::multimap<int, std::pair<int, bool>> UnorderedEdges;

class MeshSeparator
{
public:
	MeshSeparator();
	~MeshSeparator();
	
	/////////////////////////////////////////////////////////////////////////////////
	bool Process(const std::vector<Mesh>& srcMeshes_, std::vector<MeshArray>& resultMeshArrays_);
private:
	bool Process(const Mesh& srcMesh_, Mesh& resultMesh_);
	bool Slice(const Mesh& srcMesh_, BSP* root_, Mesh& resultMesh_);
	int GetGroupIdx(const Mesh& srcMesh_, BSP* root_, const Vector3& p_);


	bool BuildCrossSectionLoops2(const Mesh& resultMesh_, std::vector<Loop2>& crossSectionLoops_);
	bool BuildCrossSectionEdges2(const Mesh& resultMesh_, std::map<int, UnorderedEdges>& unorderedEdgesGroups_);
	bool BuildCrossSectionLoopsFromEdges2(std::map<int, UnorderedEdges> unorderedEdgesGroups_, std::vector<Loop2>& crossSectionLoops_);
	bool PrintCrossSectionLoops2(const Mesh& resultMesh_, const std::vector<Loop2>& loops_);
	bool AddCrossSectionLoopsToMesh2(const std::vector<Loop2>& crossSectionLoops_, Mesh& resultMesh_);


	bool BuildCrossSectionLoops(const Mesh& resultMesh_, std::vector<Loop>& crossSectionLoops_);
	bool BuildCrossSectionEdges(const Mesh& resultMesh_, int groupID_, std::multimap<int, Edge>& isolatedEdges_);
	bool BuildCrossSectionLoopsFromEdges(const std::multimap<int, Edge> isolatedEdges_, int groupID_, std::vector<Loop>& isolatedLoops_);
	bool PrintCrossSectionLoops(const Mesh& resultMesh_, const std::vector<Loop>& loops_);
	bool AddCrossSectionLoopsToMesh(const std::vector<Loop>& crossSectionLoops_, Mesh& resultMesh_);
};

#endif