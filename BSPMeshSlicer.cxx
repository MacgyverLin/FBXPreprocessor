#include "BSPMeshSlicer.h"
#include "BSP.h"

BSPMeshSlicer::BSPMeshSlicer()
{
}

BSPMeshSlicer::~BSPMeshSlicer()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
bool BSPMeshSlicer::Process(const std::vector<Mesh>& meshes, std::vector<MeshArray>& meshArrays, float crossSectionTextureScale)
{
	meshArrays.resize(meshes.size());
	for (size_t i = 0; i < meshes.size(); i++)
	{
		MeshArray cutters;
		MakeCutters(meshes[i], cutters, meshes[i].GetMaxMaterialIdx()+1, 1, crossSectionTextureScale);

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
void BSPMeshSlicer::MakeCutters(const Mesh& mesh, MeshArray& cutters, int materialID, int count, float crossSectionTextureScale)
{
	cutters.resize(count * 2);
	for (size_t i = 0; i < cutters.size(); i += 2)
	{
		Vector3 rotateAxis(Math::UnitRandom(), Math::UnitRandom(), Math::UnitRandom()); rotateAxis.Normalize();
		Vector3 center = mesh.GetAABB().GetCenter(true);
		Vector3 normal = mesh.GetAABB().GetMajorAxis(true);
		Vector3 tangent = Matrix4(rotateAxis, -90).TimesDirectionVector(normal);
		Vector3 binormal = tangent.Cross(normal);

		Vector3 extent = mesh.GetAABB().GetExtent();
		float polygonSize = Math::Max(extent.X(), Math::Max(extent.Y(), extent.Z()));
		float textureSize = polygonSize * crossSectionTextureScale;

		cutters[i + 0] = MakeCutterMesh(mesh, materialID, center, tangent, binormal, normal, polygonSize, textureSize);
		cutters[i + 1] = MakeCutterMesh(mesh, materialID, center, -tangent, binormal, -normal, polygonSize, textureSize);
	}
}

Mesh BSPMeshSlicer::MakeCutterMesh(const Mesh& mesh, int materialID, 
									const Vector3& center, const Vector3& tangent, const Vector3& binormal, const Vector3& normal, 
									float polygonSize, float textureSize)
{
	Matrix4 uvProjMatrix;
	uvProjMatrix.Init(tangent, binormal, normal, center);
	Matrix4 uvProjMatrixInv = uvProjMatrix.Inverse();

	/////////////////////////////////////////////////////////////
	std::vector<Vertex> vertices;
	Vertex v0 = MakeVertex(mesh, uvProjMatrixInv, center, normal, tangent, binormal, polygonSize, textureSize);
	Vertex v1 = MakeVertex(mesh, uvProjMatrixInv, center, normal, tangent, -binormal, polygonSize, textureSize);
	Vertex v2 = MakeVertex(mesh, uvProjMatrixInv, center, normal, -tangent, -binormal, polygonSize, textureSize);
	Vertex v3 = MakeVertex(mesh, uvProjMatrixInv, center, normal, -tangent, binormal, polygonSize, textureSize);
	vertices.push_back(v0);
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);

	Mesh result;
	result.Begin(mesh.GetColorChannelCount(), mesh.GetUVChannelCount(), mesh.GetNormalChannelCount(), mesh.GetTangentChannelCount(), mesh.GetBinormalChannelCount());
		result.BeginPolygon(0, materialID);
			result.AddPolygonVertices(vertices);
		result.EndPolygon();
	result.End();

	return result;
}

Vertex BSPMeshSlicer::MakeVertex(const Mesh& mesh, 
	const Matrix4& projMatrix,
	const Vector3& center, 
	const Vector3& normal, const Vector3& tangent, const Vector3& binormal, 
	float polygonSize, float textureSize)
{
	Vertex v;
	v.position = center + tangent * polygonSize + binormal * polygonSize;
	Vector3 vProj = projMatrix.TimesPositionVector(v.position);

	for (size_t i = 0; i < mesh.GetColorChannelCount(); i++)
		v.colors[i] = Color(1.0, 1.0, 1.0, 1.0);
	
	Vector2 uv = Vector2(vProj.X(), vProj.Y());
	uv *= Vector2(1.0f / textureSize, 1.0f / textureSize);
	for (size_t i = 0; i < mesh.GetUVChannelCount(); i++)
		v.uvs[i] = uv;

	for (size_t i = 0; i < mesh.GetNormalChannelCount(); i++)
		v.normals[i] = normal;

	for (size_t i = 0; i < mesh.GetTangentChannelCount(); i++)
		v.tangents[i] = tangent;

	for (size_t i = 0; i < mesh.GetBinormalChannelCount(); i++)
		v.binormals[i] = binormal;

	return v;
}