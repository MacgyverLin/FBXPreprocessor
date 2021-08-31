#ifndef _FBXMeshBuilder_h_
#define _FBXMeshBuilder_h_

#include "Common.h"
#include "Argument.h"
#include "Mesh.h"

class FBXMeshBuilder
{
public:
	FBXMeshBuilder();
	~FBXMeshBuilder();

	/////////////////////////////////////////////////////////////////////////////////
	bool Build(FbxScene* fbxScene, std::vector<FbxNode*>& fbxNodes, const std::vector<MeshArray>& meshArrays);
private:
	/////////////////////////////////////////////////////////////////////////////////
	bool BuildFbxMeshes(FbxScene* fbxScene, std::vector<FbxNode* >& fbxNodes, int sceneMaxMaterialIdx_, const std::vector<MeshArray>& meshArrays);
	void BuildFbxMesh(FbxScene* fbxScene, FbxNode* fbxNode, int sceneMaxMaterialIdx_, const Mesh& mesh);

	void FillColor(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, size_t ch);
	void FillUV(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, size_t ch);
	void FillNormal(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, size_t ch);
	void FillTangent(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, size_t ch);
	void FillBinormal(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, size_t ch);

	void FillPolygon(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh);

	int ComputeSceneMaxMaterialIdx(const std::vector<MeshArray>& meshArrays_);
	FbxSurfaceMaterial* AddMaterial(FbxNode* fbxNode, FbxString materialName);
};

#endif