#include "MeshSlicer.h"

MeshSlicer::MeshSlicer()
{
}

MeshSlicer::~MeshSlicer()
{
}

/////////////////////////////////////////////////////////////////////////////////
bool MeshSlicer::Slice(const std::vector<Mesh>& meshes, std::vector<MeshArray>& precutMeshArrays)
{
	if (!SliceMeshes(meshes, precutMeshArrays))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
bool MeshSlicer::SliceMeshes(const std::vector<Mesh>& meshes, std::vector<MeshArray>& precutMeshArrays)
{
	precutMeshArrays.resize(meshes.size());
	for (size_t i = 0; i < meshes.size(); i++)
	{
		Vector3 slicePlaneNormal = meshes[i].aabb.GetMajorAxis(true);
		Vector3 slicePlaneCenter = meshes[i].aabb.GetCenter(true);
		Plane slicePlane(slicePlaneNormal, slicePlaneCenter);

		precutMeshArrays[i].resize(2);
		precutMeshArrays[i][0] = Slice(meshes[i], slicePlane, slicePlaneCenter, 0.1f);

		slicePlane.Flip();

		precutMeshArrays[i][1] = Slice(meshes[i], slicePlane, slicePlaneCenter, -0.1f);
	}

	return true;
}

int MeshSlicer::GetMaxMaterialID(const Mesh& mesh) const
{
	int maxMaterialID = 0;
	for (size_t i = 0; i < mesh.polygons.size(); i++)
	{
		if (maxMaterialID < mesh.polygons[i].materialIdx)
			maxMaterialID = mesh.polygons[i].materialIdx;
	}

	return maxMaterialID;
}

Mesh MeshSlicer::Slice(const Mesh& mesh, const Plane& slicePlane, const Vector3& slicePlaneCenter, float apexOffset) const
{
	Mesh result;

	//////////////////////////////////////////////
	std::vector<Vertex> crossSectionVertices;
	for (size_t i = 0; i < mesh.polygons.size(); i++)
	{
		Polygon slicedPolygon;
		Slice(mesh.polygons[i], slicePlane, slicedPolygon, crossSectionVertices);
		if (!slicedPolygon.IsEmpty())
		{
			result.polygons.push_back(slicedPolygon);
		}
	}

	///////////////////////////
	// build cross section polygons
	Vertex apex
	(
		slicePlaneCenter + slicePlane.Normal() * apexOffset,
		slicePlane.Normal(),
		Vector2(0.0, 0.0)
	);

	int maxMaterialID = GetMaxMaterialID(mesh);

	for (size_t i = 0; i < crossSectionVertices.size(); i += 2)
	{
		Polygon crossSectionPolygon;
		crossSectionPolygon.materialIdx = maxMaterialID + 1;
		crossSectionPolygon.vertices.push_back(apex);

		bool noflip = ((crossSectionVertices[i + 1].position - apex.position).Cross(crossSectionVertices[i + 0].position - apex.position)).
			Dot(slicePlane.Normal()) > 0;

		if (noflip)
		{
			// no flip face
			crossSectionPolygon.vertices.push_back(crossSectionVertices[i + 0]);
			crossSectionPolygon.vertices.push_back(crossSectionVertices[i + 1]);
		}
		else
		{
			// flip face
			crossSectionPolygon.vertices.push_back(crossSectionVertices[i + 1]);
			crossSectionPolygon.vertices.push_back(crossSectionVertices[i + 0]);
		}
		result.polygons.push_back(crossSectionPolygon);
	}

	return result;
}

void MeshSlicer::Slice(const Polygon& polygon, const Plane& slicePlane, Polygon& result, std::vector<Vertex>& crossSectionVertices) const
{
	result.materialIdx = polygon.materialIdx;

	for (size_t i = 0; i < polygon.vertices.size(); i++)
	{
		int i0 = (i + 0) % polygon.vertices.size();
		int i1 = (i + 1) % polygon.vertices.size();
		const Vertex& v0 = polygon.vertices[i0];
		const Vertex& v1 = polygon.vertices[i1];
		bool side0 = slicePlane.WhichSide(v0.position);
		bool side1 = slicePlane.WhichSide(v1.position);

		if (side0 && side1)
		{
			result.vertices.push_back(v0);
		}
		else if (side0 && !side1)
		{
			float D = slicePlane.D();
			const Vector3& N = slicePlane.Normal();
			const Vector3& p0 = v0.position;
			const Vector3& p1 = v1.position;

			float t = (D - N.Dot(p0)) / N.Dot(p1 - p0);
			Vertex vc = Lerp(v0, v1, t);

			result.vertices.push_back(v0);
			result.vertices.push_back(vc);

			crossSectionVertices.push_back(vc);
		}
		else if (!side0 && side1)
		{
			float D = slicePlane.D();
			const Vector3& N = slicePlane.Normal();
			const Vector3& p0 = v0.position;
			const Vector3& p1 = v1.position;

			float t = (D - N.Dot(p0)) / N.Dot(p1 - p0);
			Vertex vc = Lerp(v0, v1, t);

			result.vertices.push_back(vc);

			crossSectionVertices.push_back(vc);
		}
		else if (!side0 && !side1)
		{
		}
	}
}