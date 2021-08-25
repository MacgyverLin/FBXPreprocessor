#include "MeshBuilder.h"
#include "FBXMeshBuilder.h"

#define NO_SLICE

int main(int argc, const char** argv)
{
	Arguments args(argc, argv);

	const char* fileins[] =
	{
		"1.fbx",
		"2.fbx",
		"3.fbx",
		"4.fbx",
		"5.fbx"
	};

	const char* fileouts[] =
	{
		"1out.fbx",
		"2out.fbx",
		"3out.fbx",
		"4out.fbx",
		"5out.fbx"
	};

#define TEST
#ifdef TEST
	for (int i = 0; i < 5; i++)
	//int i = 1;
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
			Debug::Error("\n\nAn error occurred while loading the fbxScene...");
			return -1;
		}

		/////////////////////////////////////////////////////////////////////
		MeshBuilder meshBuilder;
		std::vector<FbxNode*> fbxNodes;
		std::vector<Mesh> originalMeshes;
		if (!meshBuilder.Build(fbxScene, fbxNodes, originalMeshes))
		{
			Debug::Error("\n\nAn error in collect Mesh Nodes...");
			return -1;
		}

		/////////////////////////////////////////////////////////////////////
#ifdef NO_SLICE
		std::vector<MeshArray> resultMeshArrays;
		if (!meshBuilder.Copy(originalMeshes, resultMeshArrays))
		{
			Debug::Error("\n\nAn error in copying Mesh Nodes...");
			return -1;
		}
#else
#endif

		/////////////////////////////////////////////////////////////////////
		FBXMeshBuilder fbxMeshBuilder;
		if (!fbxMeshBuilder.Build(fbxScene, fbxNodes, resultMeshArrays))
		{
			Debug::Error("\n\nAn error in building fbxNodes...");
			return -1;
		}
		
		if (!SaveScene(sdkManager, fbxScene, fileout, args.binary))
		{
			Debug::Error("\n\nAn error occurred while saving the fbxScene...");
			return -1;
		}

		// Destroy all objects created by the FBX SDK.
		DestroySdkObjects(sdkManager, lResult);
	}

	return 0;
}