using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DestructionController : MonoBehaviour
{
    public List<Destruction> destructables = new List<Destruction>();

    public float explosionForce = 100.0f;
    public float explosionRadius = 10.0f;
    public float upwardsModifier = 10.0f;
    public ForceMode mode = ForceMode.Force;
    public bool doFading = true;
    public float rigidBodyMaxLifetime = 4.0f;
    public float fadeTime = 1.0f;

    // Start is void called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown("space"))
        {
            Debug.Log("space key was pressed");
            foreach (var d in destructables)
                d.GetComponent<Destruction>().Destruct(doFading, rigidBodyMaxLifetime, fadeTime, explosionForce, explosionRadius, upwardsModifier, mode);
        }
    }
}
