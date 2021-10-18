using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class JSONStringFinder
{
    public JSONStringFinder()
    {
    }

    static private int FindClosingBracketIndex(string text, int startIndex, char openedBracket = '{', char closedBracket = '}')
    {
        int index = text.IndexOf(openedBracket, startIndex);
        int bracketCount = 1;
        var textArray = text.ToCharArray();

        for (int i = index + 1; i < textArray.Length; i++)
        {
            if (textArray[i] == openedBracket)
            {
                bracketCount++;
            }
            else if (textArray[i] == closedBracket)
            {
                bracketCount--;
            }

            if (bracketCount == 0)
            {
                index = i;
                break;
            }
        }

        return index;
    }

    static public string Find(string s, string key)
    {
        int startIndex = s.IndexOf(key, 0) + key.Length;
        int endIndex = FindClosingBracketIndex(s, startIndex);

        return s.Substring(startIndex + 1, endIndex - startIndex);
    }
}

public class ModelImporterPostProcessor : AssetPostprocessor
{
    void OnPreprocessModel()
    {
        ModelImporter modelImporter = assetImporter as ModelImporter;
        modelImporter.meshOptimizationFlags = MeshOptimizationFlags.Everything;
        //modelImporter.addCollider = true;
    }

    void OnPostprocessGameObjectWithUserProperties(GameObject go, string[] propNames, System.Object[] values)
    {
        for(int i=0; i<propNames.Length; i++)
        {
            if (propNames[i].ToUpper() == "DEMOLISHABLEDATA")
            {
                //string jsonText = JSONStringFinder.Find(values[i] as string, "DemolishableData = ");
                string jsonText = values[i] as string;
                Debug.Log(jsonText);
                ModelImporter modelImporter = assetImporter as ModelImporter;
                string scriptableObjectPath = modelImporter.assetPath.Replace(".fbx", ".asset");

                DemolishableData demolishableData = AssetDatabase.LoadAssetAtPath<DemolishableData>(scriptableObjectPath);
                if (demolishableData == null)
                {
                    demolishableData = ScriptableObject.CreateInstance<DemolishableData>();
                    JsonUtility.FromJsonOverwrite(jsonText, demolishableData);

                    AssetDatabase.CreateAsset(demolishableData, scriptableObjectPath);
                }
                else
                {
                    JsonUtility.FromJsonOverwrite(jsonText, demolishableData);
                }
                
                AssetDatabase.SaveAssets();
            }
        }

        // Demolishable demolishable = go.AddComponent<Demolishable>();
    }
}