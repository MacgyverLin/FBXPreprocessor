#ifndef _MeshSeparator_h_
#define _MeshSeparator_h_

#include "Argument.h"
#include "Matrix4.h"
#include "Mesh.h"

class MeshSeparator
{
public:
	MeshSeparator();
	~MeshSeparator();
	
	/////////////////////////////////////////////////////////////////////////////////
	bool Process(const std::vector<Mesh>& srcMeshes, std::vector<MeshArray>& resultMesheArrays);
private:
	bool Process(const Mesh& srcMesh, Mesh& resultMesh);
};

#endif