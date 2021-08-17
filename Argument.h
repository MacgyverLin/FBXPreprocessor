#ifndef _Argument_h_
#define _Argument_h_

#include "../Common/Common.h"
#include <vector>
#include <map>
#include <assert.h>

class Arguments
{
public:
	Arguments(int argc, const char** argv);

	FbxString inputPath;
	FbxString outputPath;
	bool unity;
	bool unreal;
};

static void InfoMsg(char* format, ...)
{
	static char buffer[1024];
	va_list aptr;
	int ret;

	va_start(aptr, format);
	ret = vsprintf(buffer, format, aptr);
	va_end(aptr);

	FBXSDK_printf(buffer);
}

static void ErrorMsg(char* format, ...)
{
	static char buffer[1024];
	va_list aptr;
	int ret;

	va_start(aptr, format);
	ret = vsprintf(buffer, format, aptr);
	va_end(aptr);

	FBXSDK_printf(buffer);
}
static void DisplayMaterialMapping1(FbxMesh* fbxMesh)
{
	const char* lMappingTypes[] = { "None", "By Control Point", "By Polygon Vertex", "By Polygon", "By Edge", "All Same" };
	const char* lReferenceMode[] = { "Direct", "Index", "Index to Direct" };

	int lMtrlCount = 0;
	FbxNode* lNode = NULL;
	if (fbxMesh) {
		lNode = fbxMesh->GetNode();
		if (lNode)
			lMtrlCount = lNode->GetMaterialCount();
	}

	for (int l = 0; l < fbxMesh->GetElementMaterialCount(); l++)
	{
		FbxGeometryElementMaterial* leMat = fbxMesh->GetElementMaterial(l);
		if (leMat)
		{
			printf("    Material Element %d: \n", l);
			printf("           Mapping: %s \n", lMappingTypes[leMat->GetMappingMode()]);
			printf("           ReferenceMode: %s \n", lReferenceMode[leMat->GetReferenceMode()]);

			int lMaterialCount = 0;
			FbxString lString;

			if (leMat->GetReferenceMode() == FbxGeometryElement::eDirect ||
				leMat->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
			{
				lMaterialCount = lMtrlCount;
			}

			if (leMat->GetReferenceMode() == FbxGeometryElement::eIndex ||
				leMat->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
			{
				int i;

				lString = "           Indices: ";

				int lIndexArrayCount = leMat->GetIndexArray().GetCount();
				for (i = 0; i < lIndexArrayCount; i++)
				{
					lString += leMat->GetIndexArray().GetAt(i);

					if (i < lIndexArrayCount - 1)
					{
						lString += ", ";
					}
				}

				lString += "\n";

				printf(lString);
			}
		}
	}

	printf("\n");
}

#endif