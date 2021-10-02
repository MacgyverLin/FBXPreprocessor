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
        /*
        ModelImporter modelImporter = assetImporter as ModelImporter;

        string jsonPath = modelImporter.assetPath.Replace(".fbx", ".json");
        string scriptableObjectPath = modelImporter.assetPath.Replace(".fbx", ".asset");
        string scriptableObjectMetaPath = modelImporter.assetPath.Replace(".fbx", ".asset.meta");
        Debug.Log(jsonPath + ", " + scriptableObjectPath + ", " + scriptableObjectMetaPath);

        TextAsset jsonText = AssetDatabase.LoadAssetAtPath<TextAsset>(jsonPath);
        ///Debug.Log(jsonText.text);
        DemolishableData demolishableData = ScriptableObject.CreateInstance<DemolishableData>();
        JsonUtility.FromJsonOverwrite(jsonText.text, demolishableData);

        AssetDatabase.CreateAsset(demolishableData, scriptableObjectPath);
        AssetDatabase.SaveAssets();
        //AssetDatabase.DeleteAsset(jsonPath);
        */
    }

    void OnPostprocessGameObjectWithUserProperties(GameObject go, string[] propNames, System.Object[] values)
    {
        for(int i=0; i<propNames.Length; i++)
        {
            if (propNames[i] == "UDP3DSMAX")
            {
                string jsonText = JSONStringFinder.Find(values[i] as string, "DemolishableData = ");
                Debug.Log(jsonText);

                ModelImporter modelImporter = assetImporter as ModelImporter;
                string scriptableObjectPath = modelImporter.assetPath.Replace(".fbx", ".asset");

                DemolishableData demolishableData = ScriptableObject.CreateInstance<DemolishableData>();
                JsonUtility.FromJsonOverwrite(jsonText, demolishableData);

                AssetDatabase.DeleteAsset(scriptableObjectPath);
                AssetDatabase.CreateAsset(demolishableData, scriptableObjectPath);
                AssetDatabase.SaveAssets();
            }
        }

        // Demolishable demolishable = go.AddComponent<Demolishable>();
    }
}