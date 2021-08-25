#ifndef _BSPMeshSlicer_h_
#define _BSPMeshSlicer_h_

#include "Argument.h"
#include "Matrix4.h"
#include "Mesh.h"

class BSPMeshSlicer
{
public:
	BSPMeshSlicer();
	~BSPMeshSlicer();

	/////////////////////////////////////////////////////////////////////////////////
	bool Process(const std::vector<Mesh>& meshes, std::vector<MeshArray>& meshArrays, float textureSize);
private:
	bool Process(const Mesh& mesh, const MeshArray& cutters, MeshArray& meshArrays);

	void MakeCutters(const Mesh& mesh, MeshArray& cutters, int materialID, int count, float textureSize);
	Mesh MakeCutterMesh(const Mesh& mesh, int materialID, 
						const Vector3& center, const Vector3& tangent, const Vector3& binormal, const Vector3& normal, 
						float polygonSize, float textureSize);
	Vertex MakeVertex(const Mesh& mesh, 
						const Matrix4& projMatrix, const Vector3& center, 
							const Vector3& normal, const Vector3& tangent, const Vector3& binormal, float polygonSize, float textureSize);
};

#endif