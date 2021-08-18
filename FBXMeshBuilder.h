#ifndef _FBXMeshBuilder_h_
#define _FBXMeshBuilder_h_

#include "../Common/Common.h"
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

	void FillPosition(FbxMesh* dstMesh, const Mesh& mesh);
	void FillColor(bool directMode, FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillNormal(bool directMode, FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillUV(bool directMode, FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillTangent(bool directMode, FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillBinormal(bool directMode, FbxMesh* dstMesh, const Mesh& mesh, int ch);

	void FillPolygon(FbxMesh* dstMesh, const Mesh& mesh);
	void FillMaterial(FbxMesh* dstMesh, FbxNode* dstNode);
	
	/////////////////////////////////////////////////////////////////////////////////
	bool ConvertMeshesTriangleFanToTriangles(std::vector<MeshArray>& slicedMeshArrays);
};

#endif