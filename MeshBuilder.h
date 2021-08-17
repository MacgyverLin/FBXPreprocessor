#ifndef _MeshBuilder_h_
#define _MeshBuilder_h_

#include "../Common/Common.h"
#include <vector>
#include <map>
#include <assert.h>

#include "Argument.h"
#include "Geometry.h"

//////////////////////////////////////////////////////////////////////////////////////////
class MeshBuilder
{
public:
	MeshBuilder();
	~MeshBuilder();

	/////////////////////////////////////////////////////////////////////////////////
	bool Build(FbxScene* fbxScene, std::vector<FbxNode*>& fbxNodes, std::vector<Mesh>& meshes);
private:
	void CollectMeshNode(FbxScene* fbxScene, std::vector<FbxNode* >& fbxNodes);
	void CollectMeshNode(FbxNode* fbxNode, std::vector<FbxNode* >& fbxNodes);
	
	/////////////////////////////////////////////////////////////////////////////////
	bool ValidateMeshNode(FbxScene* fbxScene, FbxNode* fbxNode);
	bool ValidatePolygon(FbxMesh* fbxMesh);
	bool ValidateMeshNodes(FbxScene* fbxScene, std::vector<FbxNode* >& fbxNodes);
	
	/////////////////////////////////////////////////////////////////////////////////
	bool BuildMeshes(FbxScene* fbxScene, std::vector<FbxNode* >& fbxNodes, std::vector<Mesh >& meshes);
	bool BuildMesh(FbxNode* fbxNode, Mesh& mesh);
};

#endif