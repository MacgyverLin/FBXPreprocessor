#include "MeshBuilder.h"
#include "BSPMeshSlicer.h"
#include "FBXMeshBuilder.h"

// Todo: 
/*
TODO:
	1) Done!!!!!!!!!!! indexed color, uv, tangent, binormal

	2) Done!!!!!!!!!!! 
		mesh.colorChannelCount, mesh.normalChannelCount
			for (size_t ch = 0; ch < NUM_COLORS; ch++)...
			for (size_t ch = 0; ch < NUM_NORMALS; ch++)...

	3) Done!!!!!!!!!!! indexed position

	4) Done!!!!!!!!!!!!, Face Material Order

	5) Split plane Selection
*/

int main(int argc, const char** argv)
{
	Arguments args(argc, argv);

#define TEST
#ifdef TEST
	const char* filein[] =
	{
		"1.fbx",
		"2.fbx",
		"3.fbx",
		"4.fbx",
		"5.fbx"
	};

	const char* fileout[] =
	{
		"1out.fbx",
		"2out.fbx",
		"3out.fbx",
		"4out.fbx",
		"5out.fbx"
	};

	for (int i = 0; i < 5; i++)
#else
	int i = 0;
#endif
	{
		// Prepare the FBX SDK.
		FbxManager* sdkManager = NULL;
		FbxScene* fbxScene = NULL;
		bool lResult;
		InitializeSdkObjects(sdkManager, fbxScene);

		// Load the fbxScene.
#ifdef TEST
		lResult = LoadScene(sdkManager, fbxScene, filein[i]);
#else
		lResult = LoadScene(sdkManager, fbxScene, args.inputPath);
#endif
		if (lResult == false)
		{
			FBXSDK_printf("\n\nAn error occurred while loading the fbxScene...");
			return -1;
		}

		/////////////////////////////////////////////////////////////////////
		MeshBuilder meshBuilder;
		std::vector<FbxNode*> fbxNodes;
		std::vector<Mesh> originalMeshes;
		if (!meshBuilder.Build(fbxScene, fbxNodes, originalMeshes))
		{
			FBXSDK_printf("\n\nAn error in collect Mesh Nodes...");
			return -1;
		}

		/////////////////////////////////////////////////////////////////////
// #define NO_SLICE
#ifdef NO_SLICE
		std::vector<MeshArray> precutMeshArrays;
		if (!meshBuilder.Copy(meshes, precutMeshArrays))
		{
			FBXSDK_printf("\n\nAn error in copying Mesh Nodes...");
			return -1;
		}
#else
		BSPMeshSlicer meshSlicer;
		std::vector<MeshArray> resultMeshArrays;
		if (!meshSlicer.Process(originalMeshes, resultMeshArrays))
		{
			FBXSDK_printf("\n\nAn error in slicing Mesh Nodes...");
			return -1;
		}
#endif

		/////////////////////////////////////////////////////////////////////
		FBXMeshBuilder fbxMeshBuilder;
		if (!fbxMeshBuilder.FixMaterialOrderMeshArrays(resultMeshArrays))
		{
			FBXSDK_printf("\n\nAn error in building fbxNodes...");
			return -1;
		}

		if (!fbxMeshBuilder.TriangulateMeshArrays(resultMeshArrays))
		{
			FBXSDK_printf("\n\nAn error in building fbxNodes...");
			return -1;
		}

		if (!fbxMeshBuilder.Build(fbxScene, fbxNodes, resultMeshArrays))
		{
			FBXSDK_printf("\n\nAn error in building fbxNodes...");
			return -1;
		}
		
#ifdef TEST
		remove(fileout[i]);
		if (!SaveScene(sdkManager, fbxScene, fileout[i], args.binary))
#else
		remove(args.outputPath);
		if (!SaveScene(sdkManager, fbxScene, args.outputPath, args.binary))
#endif	
		{
			FBXSDK_printf("\n\nAn error occurred while saving the fbxScene...");
			return -1;
		}

		// Destroy all objects created by the FBX SDK.
		DestroySdkObjects(sdkManager, lResult);
	}

	return 0;
}