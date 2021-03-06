#include "MeshBuilder.h"
#include "BSPMeshSlicer.h"
#include "FBXMeshBuilder.h"

int main(int argc, const char** argv)
{
	Arguments args(argc, argv);

// #define TEST
#ifdef TEST
	const char* fileins[] =
	{
		"1.fbx",
		"2.fbx",
		"3.fbx",
		"4.fbx",
		"5.fbx",
		"6.fbx",
		"7.fbx",
	};

	const char* fileouts[] =
	{
		"1out.fbx",
		"2out.fbx",
		"3out.fbx",
		"4out.fbx",
		"5out.fbx",
		"6out.fbx",
		"7out.fbx",
	};

	for (int i = 0; i < sizeof(fileouts)/ sizeof(fileouts[0]); i++)
	//int i = 5;
#else
#endif
	{
#ifdef TEST
		const char* filein = fileins[i];
		const char* fileout = fileouts[i];
#else
		const char* filein = args.inputPath;
		const char* fileout = args.outputPath;
#endif
		remove(fileout);

		Math::RandSeed();

		// Prepare the FBX SDK.
		FbxManager* sdkManager = NULL;
		FbxScene* fbxScene = NULL;
		bool lResult;
		InitializeSdkObjects(sdkManager, fbxScene);

		// Load the fbxScene.
		lResult = LoadScene(sdkManager, fbxScene, filein);
		if (lResult == false)
		{
			ErrorMsg("\n\nAn error occurred while loading the fbxScene...");
			return -1;
		}

		/////////////////////////////////////////////////////////////////////
		MeshBuilder meshBuilder;
		std::vector<FbxNode*> fbxNodes;
		std::vector<Mesh> originalMeshes;
		if (!meshBuilder.Build(fbxScene, fbxNodes, originalMeshes))
		{
			ErrorMsg("\n\nAn error in collect Mesh Nodes...");
			return -1;
		}

		/////////////////////////////////////////////////////////////////////
		BSPMeshSlicer meshSlicer;
		std::vector<MeshArray> resultMeshArrays;
		if (!meshSlicer.Process(originalMeshes, resultMeshArrays, args.sliceCount, args.crossSectionTextureScale))
		{
			ErrorMsg("\n\nAn error in slicing Mesh Nodes...");
			return -1;
		}

		/////////////////////////////////////////////////////////////////////
		FBXMeshBuilder fbxMeshBuilder;
		//if (!fbxMeshBuilder.TriangulateMeshArrays(resultMeshArrays))
		//{
			//ErrorMsg("\n\nAn error in TriangulateMeshArrays...");
		//	return -1;
		//}

		if (!fbxMeshBuilder.Build(fbxScene, fbxNodes, resultMeshArrays))
		{
			ErrorMsg("\n\nAn error in building fbxNodes...");
			return -1;
		}
		
		if (!SaveScene(sdkManager, fbxScene, fileout, args.binary))
		{
			ErrorMsg("\n\nAn error occurred while saving the fbxScene...");
			return -1;
		}

		// Destroy all objects created by the FBX SDK.
		DestroySdkObjects(sdkManager, lResult);
	}

	return 0;
}