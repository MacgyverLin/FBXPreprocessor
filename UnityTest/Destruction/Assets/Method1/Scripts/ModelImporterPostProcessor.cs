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
        modelImporter.importAnimatedCustomProperties = true;

        string jsonPath = modelImporter.assetPath.Replace(".fbx", ".json");
        string scriptableObjectPath = modelImporter.assetPath.Replace(".fbx", ".asset");
        string scriptableObjectMetaPath = modelImporter.assetPath.Replace(".fbx", ".asset.meta");
        Debug.Log(jsonPath + ", " + scriptableObjectPath + ", " + scriptableObjectMetaPath);

        TextAsset jsonText = AssetDatabase.LoadAssetAtPath<TextAsset>(jsonPath);
        ///Debug.Log(jsonText.text);
        DemolishableData demolishableData = ScriptableObject.CreateInstance<DemolishableData>();
        JsonUtility.FromJsonOverwrite(jsonText.text, demolishableData);

        //DemolishableData demolishableData = ScriptableObject.CreateInstance<DemolishableData>();
        //demolishableData.faceGroups.Add(new FaceGroup(0, new Bound(new Vector3(1, 2, 3), new Vector3(4, 5, 6))));
        //demolishableData.faceGroups.Add(new FaceGroup(1, new Bound(new Vector3(10, 20, 30), new Vector3(40, 50, 60))));
        //demolishableData.faceGroups.Add(new FaceGroup(1, new Bound(new Vector3(11, 21, 31), new Vector3(41, 51, 61))));
        //string text = JsonUtility.ToJson(demolishableData);
        //Debug.Log(text);
        //demolishableData.faceGroups.Clear();
        //JsonUtility.FromJsonOverwrite(text, demolishableData);

        AssetDatabase.DeleteAsset(scriptableObjectMetaPath);
        AssetDatabase.DeleteAsset(scriptableObjectPath);
        AssetDatabase.CreateAsset(demolishableData, scriptableObjectPath);
        //AssetDatabase.SaveAssets();
        AssetDatabase.DeleteAsset(jsonPath);
    }
}