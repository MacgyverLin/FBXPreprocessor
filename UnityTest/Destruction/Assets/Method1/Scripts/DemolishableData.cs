using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public class Bound
{
    public Vector3 center;
    public Vector3 size;

    public Bound(Vector3 center, Vector3 size)
    {
        this.center = center;
        this.size = size;
    }
}

[System.Serializable]
public class FaceGroup
{
    public int groupID;
    public Bound bound;

    public FaceGroup(int groupID, Bound bound)
    {
        this.groupID = groupID;
        this.bound = bound;
    }
}

[CreateAssetMenu(fileName = "newData", menuName = "Tencent/CreateData", order =1)]
public class DemolishableData : ScriptableObject
{
    public List<FaceGroup> faceGroups = new List<FaceGroup>();
}