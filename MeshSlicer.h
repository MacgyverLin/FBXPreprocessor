#ifndef _MeshSlicer_h_
#define _MeshSlicer_h_

#include "../Common/Common.h"
#include <vector>
#include <map>
#include <assert.h>

#include "Argument.h"
#include "Geometry.h"

class MeshSlicer
{
public:
	MeshSlicer();
	~MeshSlicer();

	/////////////////////////////////////////////////////////////////////////////////
	bool Slice(const std::vector<Mesh>& meshes, std::vector<MeshArray>& precutMeshArrays);
private:
	/////////////////////////////////////////////////////////////////////////////////
	bool SliceMeshes(const std::vector<Mesh>& meshes, std::vector<MeshArray>& precutMeshArrays);
	int GetMaxMaterialID(const Mesh& mesh) const;

	Mesh Slice(const Mesh& mesh, const Plane& slicePlane, const Vector3& slicePlaneCenter, float apexOffset) const;
	void Slice(const Polygon& polygon, const Plane& slicePlane, Polygon& result, std::vector<Vertex>& crossSectionVertices) const;
};

#endif