#ifndef _MeshBuilder_h_
#define _MeshBuilder_h_

#include "../Common/Common.h"
#include <vector>
#include <map>
#include <assert.h>

#include "Argument.h"
#include "Mesh.h"

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

	bool BuildMaterial(FbxNode* fbxNode, Mesh& mesh, Polygon& polygon, int polygonIndex);
	bool BuildPosition(FbxNode* fbxNode, Mesh& mesh, Vertex& vertex, int lControlPointIndex, int vertexId);
	bool BuildColor(FbxNode* fbxNode, Mesh& mesh, Vertex& vertex, int lControlPointIndex, int vertexId);
	bool BuildUV(FbxNode* fbxNode, Mesh& mesh, Vertex& vertex, int lControlPointIndex, int polygonIndex, int polygonVertexIndex);
	bool BuildNormal(FbxNode* fbxNode, Mesh& mesh, Vertex& vertex, int lControlPointIndex, int vertexId);
	bool BuildTangent(FbxNode* fbxNode, Mesh& mesh, Vertex& vertex, int lControlPointIndex, int vertexId);
	bool BuildBinormal(FbxNode* fbxNode, Mesh& mesh, Vertex& vertex, int lControlPointIndex, int vertexId);
};

#endif