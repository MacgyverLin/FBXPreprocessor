#ifndef _FBXMeshBuilder_h_
#define _FBXMeshBuilder_h_

#include "../Common/Common.h"
#include <vector>
#include <map>
#include <assert.h>

#include "Argument.h"
#include "Geometry.h"

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
	void FillColor(FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillNormal(FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillUV(FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillTangent(FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillBinormal(FbxMesh* dstMesh, const Mesh& mesh, int ch);
	void FillPolygon(FbxMesh* dstMesh, const Mesh& mesh);
	void FillMaterial(FbxMesh* dstMesh, FbxNode* dstNode);
	
	/////////////////////////////////////////////////////////////////////////////////
	bool ConvertMeshesTriangleFanToTriangles(std::vector<MeshArray>& slicedMeshArrays);
	void ConvertTriangleFanToTriangles(Mesh& mesh);
	void ConvertTriangleFanToTriangles(const Polygon& polygon, std::vector<Polygon>& polygons);
};

#endif