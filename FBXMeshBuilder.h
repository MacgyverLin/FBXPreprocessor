#ifndef _FBXMeshBuilder_h_
#define _FBXMeshBuilder_h_

#include "Common.h"
#include <vector>
#include <map>
#include <assert.h>

#include "Argument.h"
#include "Mesh.h"

class FBXMeshBuilder
{
public:
	FBXMeshBuilder();
	~FBXMeshBuilder();

	/////////////////////////////////////////////////////////////////////////////////
	bool Build(FbxScene* fbxScene, std::vector<FbxNode*>& fbxNodes, std::vector<MeshArray>& precutMeshArrays);
private:
	/////////////////////////////////////////////////////////////////////////////////
	bool BuildFbxMeshes(FbxScene* fbxScene, std::vector<FbxNode* >& fbxNodes, std::vector<MeshArray>& precutMeshArrays);
	FbxNode* BuildFbxMesh(FbxScene* fbxScene, FbxNode* fbxNode, const Mesh& mesh, const FbxString& name);

	void FillColor(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillUV(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillNormal(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillTangent(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillBinormal(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, int ch);

	void FillPosition(FbxMesh* dstMesh, const Mesh& mesh);
	void FillPolygon(FbxMesh* dstMesh, const Mesh& mesh);
	void FillPolygon2(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh);

	void FillMaterial(FbxMesh* dstMesh, FbxNode* dstNode);
	
	/////////////////////////////////////////////////////////////////////////////////
	bool ConvertMeshesTriangleFanToTriangles(std::vector<MeshArray>& slicedMeshArrays);
};

#endif