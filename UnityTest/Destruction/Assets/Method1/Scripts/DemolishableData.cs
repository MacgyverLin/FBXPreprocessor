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
public class BoundCapsule
{
    public Vector3 center;
    public float height;
    public float radius;
    public Vector3 direction;

    public BoundCapsule(Vector3 center, float height, float radius, Vector3 direction)
    {
        this.center = center;
        this.height = height;
        this.radius = radius;
        this.direction = direction;
    }
}

[System.Serializable]
public class FaceGroup
{
    public int groupID;
    public Bound bound;
    public BoundCapsule boundCapsule;

    public FaceGroup(int groupID, Bound bound, BoundCapsule boundCapsule)
    {
        this.groupID = groupID;
        this.bound = bound;
        this.boundCapsule = boundCapsule;
    }
}

[CreateAssetMenu(fileName = "newData", menuName = "Tencent/CreateData", order =1)]
public class DemolishableData : ScriptableObject
{
    public List<FaceGroup> faceGroups = new List<FaceGroup>();

    public int GetFaceGroupCount()
    {
        return faceGroups.Count;
    }

    public FaceGroup GetFaceGroup(int i)
    {
        return faceGroups[i];
    }
}