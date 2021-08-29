/****************************************************************************************

   Copyright (C) 2015 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

#include "Common.h"
 
#ifdef IOS_REF
	#undef  IOS_REF
	#define IOS_REF (*(pManager->GetIOSettings()))
#endif

void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
    //The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
    pManager = FbxManager::Create();
    if( !pManager )
    {
        // Debug::Error("Error: Unable to create FBX Manager!\n");
        exit(1);
    }
    else
    {
        // Debug::Info("Autodesk FBX SDK version %s\n", pManager->GetVersion());
    }

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer());

    //Create an FBX scene. This object holds most objects imported/exported from/to files.
    pScene = FbxScene::Create(pManager, "My Scene");
	if( !pScene )
    {
        // Debug::Error("Error: Unable to create FBX scene!\n");
        exit(1);
    }
}

void DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
    //Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
    if( pManager ) 
        pManager->Destroy();
    if (pExitStatus)
    {
        // Debug::Info("Program Success!\n");
    }
}

bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, bool binary)
{
    int lMajor, lMinor, lRevision;
    bool lStatus = true;

    // Create an exporter.
    FbxExporter* lExporter = FbxExporter::Create(pManager, "");

    int pFileFormat = -1;
    if( pFileFormat < 0 || pFileFormat >= pManager->GetIOPluginRegistry()->GetWriterFormatCount() )
    {
        // Write in fall back format in less no ASCII format found
        pFileFormat = pManager->GetIOPluginRegistry()->GetNativeWriterFormat();

        //Try to export in ASCII if possible
        int lFormatIndex, lFormatCount = pManager->GetIOPluginRegistry()->GetWriterFormatCount();

        for (lFormatIndex=0; lFormatIndex<lFormatCount; lFormatIndex++)
        {
            if (pManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
            {
                FbxString lDesc =pManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
                if (binary && lDesc.Find("FBX binary")>=0)
                {
                    pFileFormat = lFormatIndex;
                    break;
                }
                if (!binary && lDesc.Find("FBX ascii") >= 0)
                {
                    pFileFormat = lFormatIndex;
                    break;
                }
            }
        } 
    }

    // Set the export states. By default, the export states are always set to 
    // true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
    // shows how to change these states.
    IOS_REF.SetBoolProp(EXP_FBX_MATERIAL,        true);
    IOS_REF.SetBoolProp(EXP_FBX_TEXTURE,         true);
    IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED,        false);
    IOS_REF.SetBoolProp(EXP_FBX_SHAPE,           true);
    IOS_REF.SetBoolProp(EXP_FBX_GOBO,            true);
    IOS_REF.SetBoolProp(EXP_FBX_ANIMATION,       true);
    IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

    // Initialize the exporter by providing a filename.
    if(lExporter->Initialize(pFilename, pFileFormat, pManager->GetIOSettings()) == false)
    {
        // Debug::Error("Call to FbxExporter::Initialize() failed.\n");
        // Debug::Error("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
        return false;
    }

    FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
    // Debug::Info("FBX file format version %d.%d.%d\n\n", lMajor, lMinor, lRevision);

    // Export the scene.
    lStatus = lExporter->Export(pScene); 

    // Destroy the exporter.
    lExporter->Destroy();
    return lStatus;
}

bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    //int lFileFormat = -1;
    int lAnimStackCount;
    bool lStatus;
    char lPassword[1024];

    // Get the file version number generate by the FBX SDK.
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    // Create an importer.
    FbxImporter* lImporter = FbxImporter::Create(pManager,"");

    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if( !lImportStatus )
    {
        FbxString error = lImporter->GetStatus().GetErrorString();
        // Debug::Error("Call to FbxImporter::Initialize() failed.\n");
        // Debug::Error("Error returned: %s\n\n", error.Buffer());

        if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
        {
            // Debug::Error("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
            // Debug::Error("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
        }

        return false;
    }

    // Debug::Info("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

    if (lImporter->IsFBX())
    {
        // Debug::Info("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

        // From this point, it is possible to access animation stack information without
        // the expense of loading the entire file.

        // Debug::Info("Animation Stack Information\n");

        lAnimStackCount = lImporter->GetAnimStackCount();

        // Debug::Info("    Number of Animation Stacks: %d\n", lAnimStackCount);
        // Debug::Info("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
        // Debug::Info("\n");

        for(int i = 0; i < lAnimStackCount; i++)
        {
            FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

            // Debug::Info("    Animation Stack %d\n", i);
            // Debug::Info("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
            // Debug::Info("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

            // Change the value of the import name if the animation stack should be imported 
            // under a different name.
            // Debug::Info("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

            // Set the value of the import state to false if the animation stack should be not
            // be imported. 
            // Debug::Info("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
            // Debug::Info("\n");
        }

        // Set the import states. By default, the import states are always set to 
        // true. The code below shows how to change these states.
        IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
        IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
        IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
        IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
        IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
        IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
        IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = lImporter->Import(pScene);
	if (lStatus == false && lImporter->GetStatus() == FbxStatus::ePasswordError)
	{
		FBXSDK_printf("Please enter password: ");

		lPassword[0] = '\0';

		FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
			scanf("%s", lPassword);
		FBXSDK_CRT_SECURE_NO_WARNING_END

			FbxString lString(lPassword);

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(pScene);

		if (lStatus == false && lImporter->GetStatus() == FbxStatus::ePasswordError)
		{
            // Debug::Error("\nPassword is wrong, import aborted.\n");
		}
	}

	if (!lStatus || (lImporter->GetStatus() != FbxStatus::eSuccess))
	{
        // Debug::Info("********************************************************************************\n");
		if (lStatus)
		{
            // Debug::Error("WARNING:\n");
            // Debug::Error("   The importer was able to read the file but with errors.\n");
            // Debug::Error("   Loaded scene may be incomplete.\n\n");
		}
		else
		{
            // Debug::Error("Importer failed to load the file!\n\n");
		}

        if (lImporter->GetStatus() != FbxStatus::eSuccess)
        {
            // Debug::Error("   Last error message: %s\n", lImporter->GetStatus().GetErrorString());
        }

		FbxArray<FbxString*> history;
		lImporter->GetStatus().GetErrorStringHistory(history);
		if (history.GetCount() > 1)
		{
            // Debug::Info("   Error history stack:\n");
			for (int i = 0; i < history.GetCount(); i++)
			{
                // Debug::Info("      %s\n", history[i]->Buffer());
			}
		}
		FbxArrayDelete<FbxString*>(history);
        // Debug::Info("********************************************************************************\n");
	}

    // Destroy the importer.
    lImporter->Destroy();

    return lStatus;
}

///////////////////////////////////////////////////////////
static FILE* fptr = nullptr;

bool Debug::Init()
{
    fptr = fopen("log.txt", "wt");
    
    return fptr!=nullptr;
}

void Debug::Terminate()
{
    if (fptr)
    {
        fclose(fptr);
        fptr = nullptr;
    }
}

void Debug::Log(const char* buffer)
{
    assert(fptr);

    fprintf(fptr, buffer);
}

void Debug::Print(const char* buffer)
{
    FBXSDK_printf(buffer);
}

void Debug::Verbose(const char* format, ...)
{
    static char buffer[4096];
    va_list aptr;
    int ret;

    va_start(aptr, format);
    ret = vsprintf(buffer, format, aptr);
    va_end(aptr);

    // Debug::Print("Verbose: ");
    Debug::Print(buffer);
}

void Debug::Info(const char* format, ...)
{
    static char buffer[4096];
    va_list aptr;
    int ret;

    va_start(aptr, format);
    ret = vsprintf(buffer, format, aptr);
    va_end(aptr);

    // Debug::Print("Info: ");
    Debug::Print(buffer);
}

void Debug::Warning(const char* format, ...)
{
    static char buffer[4096];
    va_list aptr;
    int ret;

    va_start(aptr, format);
    ret = vsprintf(buffer, format, aptr);
    va_end(aptr);

    // Debug::Print("Warning: ");
    Debug::Print(buffer);
}

void Debug::Error(const char* format, ...)
{
    static char buffer[4096];
    va_list aptr;
    int ret;

    va_start(aptr, format);
    ret = vsprintf(buffer, format, aptr);
    va_end(aptr);

    // Debug::Print("Error: ");
    Debug::Print(buffer);
}

void Debug::Fatal(const char* format, ...)
{
    static char buffer[4096];
    va_list aptr;
    int ret;

    va_start(aptr, format);
    ret = vsprintf(buffer, format, aptr);
    va_end(aptr);

    //  Debug::Print("Fatal: ");
    Debug::Print(buffer);
    exit(-1);
}
