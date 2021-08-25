#ifndef _Argument_h_
#define _Argument_h_

#include <vector>
#include <map>
#include <assert.h>
#include "Common.h"

class Arguments
{
public:
	Arguments(int argc, const char** argv);

	FbxString inputPath;
	FbxString outputPath;
	bool binary;
	bool unity;
	bool unreal;
	float crossSectionTextureScale;
	int sliceCount;
};

extern void InfoMsg(char* format, ...);
extern void ErrorMsg(char* format, ...);
extern void DisplayMaterialMapping1(FbxMesh* fbxMesh);

#endif