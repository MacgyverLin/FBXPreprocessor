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
	void FillDirectColor(FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillDirectNormal(FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillDirectUV(FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillDirectTangent(FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillDirectBinormal(FbxMesh* dstMesh, const Mesh& mesh, int ch);

	void FillIndexedColor(FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillIndexedNormal(FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillIndexedUV(FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillIndexedTangent(FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillIndexedBinormal(FbxMesh* dstMesh, const Mesh& mesh, int ch);

	void FillPolygon(FbxMesh* dstMesh, const Mesh& mesh);
	void FillMaterial(FbxMesh* dstMesh, FbxNode* dstNode);
	
	/////////////////////////////////////////////////////////////////////////////////
	bool ConvertMeshesTriangleFanToTriangles(std::vector<MeshArray>& slicedMeshArrays);
};

#endif