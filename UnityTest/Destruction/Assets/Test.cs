using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Test : MonoBehaviour
{
    public int maxNeighbours = 10; // maximum amount of neighbours visualised
    private Collider[] neighbours;
    Collider thisCollider;

    public float isDestructed = 1.0f;

    // Start is called before the first frame update
    void Start()
    {
        neighbours = new Collider[maxNeighbours];
        thisCollider = GetComponent<Collider>();
    }

    bool ComputePenetration(Transform transform, float radius, out Vector3 penetrateDirection, out float penetrateDistance)
    {
        penetrateDirection = Vector3.zero;
        penetrateDistance = 0;
        if (!thisCollider)
        {
            return false;
        }
        else
        {
            bool overlapped = false;

            int count = Physics.OverlapSphereNonAlloc(transform.position, radius, neighbours);
            for (int i = 0; i < count; ++i)
            {
                var collider = neighbours[i];
                if (collider == thisCollider)
                    continue; // skip myself

                Vector3 otherPosition = collider.gameObject.transform.position;
                Quaternion otherRotation = collider.gameObject.transform.rotation;

                Vector3 dir;
                float dist;
                overlapped = Physics.ComputePenetration(
                    thisCollider, transform.position, transform.rotation,
                    collider, otherPosition, otherRotation,
                    out dir, out dist
                );

                if (overlapped)
                {
                    penetrateDirection += dir * dist;
                }
            }

            if (overlapped)
            {
                penetrateDistance = penetrateDirection.magnitude;
                penetrateDirection = penetrateDirection.normalized;
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    // Update is called once per frame
    void Update()
    {
        //MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        //for (int i = 0; i < meshRenderer.materials.Length; i++)
        //{
            //meshRenderer.materials[i].SetFloat("_IsDestructed", isDestructed);
        //}

        /*
        Vector3 direction;
        float distance;
        if (ComputePenetration(transform, 1.0f, out direction, out distance))
        {
            transform.position += direction * distance;
        }
        */
    }
}
