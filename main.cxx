#include "MeshBuilder.h"
#include "FBXMeshBuilder.h"
#include "MeshSeparator.h"

#include "Common.h"
#include "Argument.h"
#include "Mesh.h"

void ProcessMeshNode(FbxNode* fbxNode)
{
	FbxNodeAttribute::EType lAttributeType;
	FbxMesh* mesh;

	if (fbxNode->GetNodeAttribute() == NULL)
	{
		FBXSDK_printf("NULL Node Attribute\n\n");
	}
	else
	{
		lAttributeType = (fbxNode->GetNodeAttribute()->GetAttributeType());

		switch (lAttributeType)
		{
		case FbxNodeAttribute::eMesh:
			mesh = fbxNode->GetMesh();
			mesh->SetUserDataPtr("test 1");
			break;

		case FbxNodeAttribute::eMarker:
		case FbxNodeAttribute::eSkeleton:
		case FbxNodeAttribute::eNurbs:
		case FbxNodeAttribute::ePatch:
		case FbxNodeAttribute::eCamera:
		case FbxNodeAttribute::eLight:
		case FbxNodeAttribute::eLODGroup:
		default:
			break;
		}
	}

	//DisplayUserProperties(pNode);
	//DisplayTarget(pNode);
	//DisplayPivotsAndLimits(pNode);
	//DisplayTransformPropagation(pNode);
	//DisplayGeometricTransform(pNode);

	for (int i = 0; i < fbxNode->GetChildCount(); i++)
	{
		ProcessMeshNode(fbxNode->GetChild(i));
	}
}

void ProcessSceneMeshNode(FbxScene* fbxScene)
{
	fbxScene->SetUserDataPtr("test 2");

	FbxNode* fbxNode = fbxScene->GetRootNode();

	if (fbxNode)
	{
		for (int i = 0; i < fbxNode->GetChildCount(); i++)
		{
			ProcessMeshNode(fbxNode->GetChild(i));
		}
	}
}


int main(int argc, const char** argv)
{
	Arguments args(argc, argv);

#define TEST
#ifdef TEST
	const char* fileins[] =
	{
		"0.fbx",
		"1.fbx",
		"1open.fbx",
		"2.fbx",
		"3.fbx",
		"4.fbx",
		"5.fbx",
		"6.fbx"
	};

	const char* fileouts[] =
	{
		"0.fbx",
		"1.fbx",
		"1open.fbx",
		"2.fbx",
		"3.fbx",
		"4.fbx",
		"5.fbx",
		"6.fbx"
	};

	for (int i = 0; i < sizeof(fileouts) / sizeof(fileouts[0]); i++)
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
		//remove(fileout);

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

		ProcessSceneMeshNode(fbxScene);

		/*
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
		MeshSeparator meshSeparator;
		std::vector<MeshArray> resultMeshArrays;
		if (!meshSeparator.Process(originalMeshes, resultMeshArrays))
		{
			Debug::Error("\n\nAn error in processing Mesh Nodes...");
			return -1;
		}

		/////////////////////////////////////////////////////////////////////
		FBXMeshBuilder fbxMeshBuilder;
		if (!fbxMeshBuilder.Build(fbxScene, fbxNodes, resultMeshArrays))
		{
			Debug::Error("\n\nAn error in building fbxNodes...");
			return -1;
		}
		*/
		
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