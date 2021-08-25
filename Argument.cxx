#include "Argument.h"

Arguments::Arguments(int argc, const char** argv)
{
	inputPath = "";
	outputPath = "out.fbx"; 
	binary = true;
	unity = true;
	unreal = false;
	crossSectionTextureScale = 1.0f;
	sliceCount = 1;
	 
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-') 
		{
			if (stricmp(&argv[i][1], "o") == 0 || stricmp(&argv[i][1], "out") == 0)
			{
				i++;
				outputPath = argv[i];
			}
			else if (stricmp(&argv[i][1], "a") == 0 || stricmp(&argv[i][1], "ascii") == 0)
			{
				binary = false;
			}
			else if (stricmp(&argv[i][1], "cts") == 0 || stricmp(&argv[i][1], "cross_section_texture_scale") == 0)
			{
				i++;
				crossSectionTextureScale = atof(argv[i]);
			}
			else if (stricmp(&argv[i][1], "sc") == 0 || stricmp(&argv[i][1], "slice_count") == 0)
			{
				i++;
				sliceCount = atof(argv[i]);
			}
			else if (stricmp(&argv[i][1], "engine") == 0)
			{
				i++;
				if (stricmp(argv[i], "unreal") == 0)
				{
					unreal = true;
				}
				else if (stricmp(argv[i], "unity") == 0)
				{
					unity = true;
				}
				else
				{
					unity = true;
				}
			}
		}
		else
		{
			inputPath = argv[i];
		}
	}
}

void InfoMsg(char* format, ...)
{
	static char buffer[1024];
	va_list aptr;
	int ret;

	va_start(aptr, format);
	ret = vsprintf(buffer, format, aptr);
	va_end(aptr);

	FBXSDK_printf(buffer);
}

void ErrorMsg(char* format, ...)
{
	static char buffer[1024];
	va_list aptr;
	int ret;

	va_start(aptr, format);
	ret = vsprintf(buffer, format, aptr);
	va_end(aptr);

	FBXSDK_printf(buffer);
}

void DisplayMaterialMapping1(FbxMesh* fbxMesh)
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