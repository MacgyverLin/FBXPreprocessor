#include "BSPMeshSlicer.h"
#include "BSP.h"

BSPMeshSlicer::BSPMeshSlicer()
{
}

BSPMeshSlicer::~BSPMeshSlicer()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
bool BSPMeshSlicer::Process(const std::vector<Mesh>& meshes, std::vector<MeshArray>& meshArrays)
{
	meshArrays.resize(meshes.size());
	for (size_t i = 0; i < meshes.size(); i++)
	{
		MeshArray cutters;
		MakeCutters(meshes[i], cutters, meshes[i].maxMaterialIdx+1, 1);

		if (!Process(meshes[i], cutters, meshArrays[i]))
			return false;
	}

	return true;
}

bool BSPMeshSlicer::Process(const Mesh& mesh, const MeshArray& cutters, MeshArray& meshArrays)
{
	meshArrays.resize(cutters.size());
	for (size_t i = 0; i < cutters.size(); i++)
	{
		meshArrays[i] = Intersect(mesh, cutters[i]);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
void BSPMeshSlicer::MakeCutters(const Mesh& mesh, MeshArray& cutters, int materialID, int count)
{
	cutters.resize(count * 2);
	for (size_t i = 0; i < cutters.size(); i += 2)
	{
		Polygon p = MakeCutterPolygon(mesh, materialID, true, true);

		cutters[i + 0].Add(p);
		cutters[i + 0].maxMaterialIdx = materialID;

		p.Flip();

		cutters[i + 1].Add(p);
		cutters[i + 0].maxMaterialIdx = materialID;
	}
}

Polygon BSPMeshSlicer::MakeCutterPolygon(const Mesh& mesh, int materialID, bool randomizeNormal, bool randomizeCenter)
{
	Vector3 rotateAxis(Math::UnitRandom(), Math::UnitRandom(), Math::UnitRandom()); rotateAxis.Normalize();

	Vector3 normal = mesh.GetAABB().GetMajorAxis(randomizeNormal);
	Vector3 center = mesh.GetAABB().GetCenter(randomizeCenter);
	Vector3 tangent = Matrix4(rotateAxis, 90).TimesDirectionVector(normal);
	Vector3 binormal = normal.Cross(tangent);

	Matrix4 uvProjMatrix;
	uvProjMatrix.Init(tangent, binormal, normal, center);
	Matrix4 uvProjMatrixInv = uvProjMatrix.Inverse();

	/////////////////////////////////////////////////////////////
	std::vector<Vertex> vertices;
	vertices.push_back(MakeVertex(mesh, center, normal, tangent, binormal, 1000.0f, uvProjMatrixInv, 1.0f));
	vertices.push_back(MakeVertex(mesh, center, normal, tangent, -binormal, 1000.0f, uvProjMatrixInv, 1.0f));
	vertices.push_back(MakeVertex(mesh, center, normal, -tangent, -binormal, 1000.0f, uvProjMatrixInv, 1.0f));
	vertices.push_back(MakeVertex(mesh, center, normal, -tangent, binormal, 1000.0f, uvProjMatrixInv, 1.0f));

	return Polygon(materialID, vertices);
}

Vertex BSPMeshSlicer::MakeVertex(const Mesh& mesh, const Vector3& center, const Vector3& normal, const Vector3& tangent, const Vector3& binormal, float polygonSize,
	const Matrix4& projMatrix, float uvScale)
{
	Vertex v;
	v.position = center + tangent * polygonSize + binormal * polygonSize;
	Vector3 vProj = projMatrix.TimesPositionVector(v.position);

	for (size_t i = 0; i < mesh.colorChannelCount; i++)
		v.colors[i] = Color(1.0, 1.0, 1.0, 1.0);

	for (size_t i = 0; i < mesh.uvChannelCount; i++)
		v.uvs[i] = Vector2(vProj.X(), vProj.Y()) * uvScale;

	for (size_t i = 0; i < mesh.normalChannelCount; i++)
		v.normals[i] = normal;

	for (size_t i = 0; i < mesh.tangentChannelCount; i++)
		v.tangents[i] = tangent;

	for (size_t i = 0; i < mesh.binormalChannelCount; i++)
		v.binormals[i] = binormal;

	return v;
}