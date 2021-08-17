#include "BSPMeshSlicer.h"

BSPMeshSlicer::BSPMeshSlicer()
{
}

BSPMeshSlicer::~BSPMeshSlicer()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
bool BSPMeshSlicer::Slice(const std::vector<Mesh>& meshes, std::vector<MeshArray>& precutMeshArrays)
{
	precutMeshArrays.resize(meshes.size());
	for (size_t i = 0; i < meshes.size(); i++)
	{
		MeshArray cutters;
		MakeCutters(meshes[i], cutters, 3, 1);

		if (!Slice(meshes[i], cutters, precutMeshArrays[i]))
			return false;
	}

	return true;
}

bool BSPMeshSlicer::Slice(const Mesh& mesh, const MeshArray& cutters, MeshArray& slicedMeshArray)
{
	slicedMeshArray.resize(cutters.size());
	for (size_t i = 0; i < cutters.size(); i++)
	{
		slicedMeshArray[i] = Slice(mesh, cutters[i]);
	}

	return true;
}

Mesh BSPMeshSlicer::Slice(const Mesh& mesh, const Mesh& cutter) const
{
	return Intersect(mesh, cutter);
}

//////////////////////////////////////////////////////////////////////////////////
void BSPMeshSlicer::MakeCutters(const Mesh& mesh, MeshArray& cutters, int materialID, int count)
{
	cutters.resize(count * 2);
	for (size_t i = 0; i < cutters.size(); i += 2)
	{
		Polygon p = MakeCutterPolygon(mesh, materialID);

		cutters[i + 0].Add(p);

		p.Flip();

		cutters[i + 1].Add(p);
	}
}

Polygon BSPMeshSlicer::MakeCutterPolygon(const Mesh& mesh, int materialID)
{
	Vector3 rotateAxis(UnitRandom(), UnitRandom(), UnitRandom()); rotateAxis.Normalize();

	Vector3 normal = mesh.aabb.GetMajorAxis(true);
	Vector3 center = mesh.aabb.GetCenter(true);
	Vector3 tangent = Matrix4(rotateAxis, 90).TimesDirectionVector(normal);
	Vector3 binormal = normal.Cross(tangent);

	Matrix4 uvProjMatrix;
	uvProjMatrix.Init(tangent, binormal, normal, center);
	Matrix4 uvProjMatrixInv = uvProjMatrix.Inverse();

	/////////////////////////////////////////////////////////////
	std::vector<Vertex> vertices;
	vertices.push_back(MakeVertex(center, normal, tangent, binormal, 1000.0f, uvProjMatrixInv, 1.0f));
	vertices.push_back(MakeVertex(center, normal, tangent, -binormal, 1000.0f, uvProjMatrixInv, 1.0f));
	vertices.push_back(MakeVertex(center, normal, -tangent, -binormal, 1000.0f, uvProjMatrixInv, 1.0f));
	vertices.push_back(MakeVertex(center, normal, -tangent, binormal, 1000.0f, uvProjMatrixInv, 1.0f));

	return Polygon(materialID, vertices);
}

Vertex BSPMeshSlicer::MakeVertex(const Vector3& center, const Vector3& normal, const Vector3& tangent, const Vector3& binormal, float polygonSize,
	const Matrix4& projMatrix, float uvScale)
{
	Vertex v;
	v.position = center + tangent * polygonSize + binormal * polygonSize;
	Vector3 vProj = projMatrix.TimesPositionVector(v.position);

	for (size_t i = 0; i < NUM_COLORS; i++)
		v.colors[i] = Color(1.0, 1.0, 1.0, 1.0);

	for (size_t i = 0; i < NUM_UVS; i++)
		v.uvs[i] = Vector2(vProj.X(), vProj.Y()) * uvScale;

	for (size_t i = 0; i < NUM_NORMALS; i++)
		v.normals[i] = normal;

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		v.tangents[i] = tangent;

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		v.binormals[i] = binormal;

	return v;
}