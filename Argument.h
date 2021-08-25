#ifndef _Argument_h_
#define _Argument_h_

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
};

#endif