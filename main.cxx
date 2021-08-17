#include "MeshBuilder.h"
#include "MeshSlicer.h"
#include "BSPMeshSlicer.h"
#include "FBXMeshBuilder.h"

int main(int argc, const char** argv)
{
	Arguments params(argc, argv);

	// Prepare the FBX SDK.
	FbxManager* sdkManager = NULL;
	FbxScene* fbxScene = NULL;
	bool lResult;
	InitializeSdkObjects(sdkManager, fbxScene);

	// Load the fbxScene.
	lResult = LoadScene(sdkManager, fbxScene, params.inputPath);
	if (lResult == false)
	{
		FBXSDK_printf("\n\nAn error occurred while loading the fbxScene...");
		return -1;
	}

	MeshBuilder meshBuilder;
	std::vector<FbxNode*> fbxNodes;
	std::vector<Mesh> meshes;
	if (!meshBuilder.Build(fbxScene, fbxNodes, meshes))
	{
		FBXSDK_printf("\n\nAn error in collect Mesh Nodes...");
		return -1;
	}

	//MeshSlicer meshSlicer;
	BSPMeshSlicer meshSlicer;
	std::vector<MeshArray> precutMeshArrays;
	if(!meshSlicer.Slice(meshes, precutMeshArrays))
	{
		FBXSDK_printf("\n\nAn error in slicing Mesh Nodes...");
		return -1;
	}

	FBXMeshBuilder fbxMeshBuilder;
	if (!fbxMeshBuilder.Build(fbxScene, fbxNodes, precutMeshArrays))
	{
		FBXSDK_printf("\n\nAn error in building fbxNodes...");
		return -1;
	}

	remove(params.outputPath);
	if (!SaveScene(sdkManager, fbxScene, params.outputPath))
	{
		FBXSDK_printf("\n\nAn error occurred while saving the fbxScene...");
		return -1;
	}

	// Destroy all objects created by the FBX SDK.
	DestroySdkObjects(sdkManager, lResult);

	return 0;
}