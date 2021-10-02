using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

// Disable import of materials if the file contains
// the @ sign marking it as an animation.
public class ModelImporterPostProcessor : AssetPostprocessor
{
    void OnPreprocessModel()
    {
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
    }

    void OnPostprocessGameObjectWithUserProperties(GameObject go, string[] propNames, System.Object[] values)
    {
        for(int i=0; i<propNames.Length; i++)
        {
            Debug.Log(propNames[i] + ":" + values[i]);
        }

        Demolishable demolishable = go.AddComponent<Demolishable>();
    }
}