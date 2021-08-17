#include "Argument.h"

Arguments::Arguments(int argc, const char** argv)
{
	inputPath = "";
	outputPath = "out.fbx"; 
	unity = true;
	unreal = false;

	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-') 
		{
			if (stricmp(&argv[i][1], "o") == 0 || stricmp(&argv[i][1], "out") == 0)
			{
				i++;
				outputPath = argv[i];
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