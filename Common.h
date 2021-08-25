/****************************************************************************************

   Copyright (C) 2015 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/
#ifndef _COMMON_H
#define _COMMON_H

#include <fbxsdk.h>
#include <functional>
#include <vector>
#include <map>
#include <assert.h>

void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);
void CreateAndFillIOSettings(FbxManager* pManager);

bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, bool binary);
bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);

class Debug
{
public:
	static void Verbose(char* format, ...);
	static void Info(char* format, ...);
	static void Warning(char* format, ...);
	static void Error(char* format, ...);
	static void Fatal(char* format, ...);
};

#endif // #ifndef _COMMON_H


