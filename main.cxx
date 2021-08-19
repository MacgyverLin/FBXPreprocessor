#include "MeshBuilder.h"
#include "BSPMeshSlicer.h"
#include "FBXMeshBuilder.h"

int main(int argc, const char** argv)
{
	Arguments args(argc, argv);

	const char* filein[] =
	{
		"1.fbx",
		"2.fbx",
		"3.fbx",
		"4.fbx"
	};

	const char* fileout[] =
	{
		"1out.fbx",
		"2out.fbx",
		"3out.fbx",
		"4out.fbx"
	};

	for (int i = 3; i < 4; i++)
	{
		// Prepare the FBX SDK.
		FbxManager* sdkManager = NULL;
		FbxScene* fbxScene = NULL;
		bool lResult;
		InitializeSdkObjects(sdkManager, fbxScene);

		// Load the fbxScene.
		lResult = LoadScene(sdkManager, fbxScene, filein[i]); // args.inputPath
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

		BSPMeshSlicer meshSlicer;
		std::vector<MeshArray> precutMeshArrays;
		//if (!meshSlicer.Copy(meshes, precutMeshArrays))
		if (!meshSlicer.Slice(meshes, precutMeshArrays))
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
		
		remove(fileout[i]);
		//if (!SaveScene(sdkManager, fbxScene, args.outputPath, args.binary))
		if (!SaveScene(sdkManager, fbxScene, fileout[i], true))
		{
			FBXSDK_printf("\n\nAn error occurred while saving the fbxScene...");
			return -1;
		}

		// Destroy all objects created by the FBX SDK.
		DestroySdkObjects(sdkManager, lResult);
	}

	return 0;
}