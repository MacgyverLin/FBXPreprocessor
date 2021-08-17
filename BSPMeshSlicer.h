#ifndef _BSPMeshSlicer_h_
#define _BSPMeshSlicer_h_

#include "../Common/Common.h"
#include <vector>
#include <map>
#include <assert.h>

#include "Argument.h"
#include "Matrix4.h"
#include "Mesh.h"

class BSPMeshSlicer
{
public:
	BSPMeshSlicer();
	~BSPMeshSlicer();

	/////////////////////////////////////////////////////////////////////////////////
	bool Slice(const std::vector<Mesh>& meshes, std::vector<MeshArray>& precutMeshArrays);
private:
	bool Slice(const Mesh& mesh, const MeshArray& cutters, MeshArray& slicedMeshArray);
	Mesh Slice(const Mesh& mesh, const Mesh& cutter) const;

	void MakeCutters(const Mesh& mesh, MeshArray& cutters, int materialID, int count);
	Polygon MakeCutterPolygon(const Mesh& mesh, int materialID);
	Vertex MakeVertex(const Vector3& center, const Vector3& normal, const Vector3& tangent, const Vector3& binormal, float polygonSize,
		const Matrix4& projMatrix, float uvScale);
};

#endif