using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DemolishController : MonoBehaviour
{
    public List<Demolishable> destructables = new List<Demolishable>();

    public float explosionForce = 100.0f;
    public float explosionRadius = 10.0f;
    public float upwardsModifier = 10.0f;
    public ForceMode mode = ForceMode.Force;
    public bool doFading = true;
    public float rigidBodyMaxLifetime = 4.0f;
    public float fadeTime = 1.0f;

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown("space"))
        {
            Debug.Log("space key was pressed");
            Demolish(doFading, rigidBodyMaxLifetime, fadeTime, explosionForce, explosionRadius, upwardsModifier, mode);
        }
    }

    public void Demolish(bool doFading, float rigidBodyMaxLifetime, float fadeTime, float explosionForce, float explosionRadius, float upwardsModifier = 0.0f, ForceMode mode = ForceMode.Force)
    {
        StartCoroutine(DemolishCoroutine(doFading, rigidBodyMaxLifetime, fadeTime, explosionForce, explosionRadius, upwardsModifier, mode));
    }

    private IEnumerator DemolishCoroutine(bool doFading, float rigidBodyMaxLifetime, float fadeTime, float explosionForce, float explosionRadius, float upwardsModifier, ForceMode mode)
    {
        int i = 0;

        while (i < destructables.Count)
        {
            if (destructables[i].isActiveAndEnabled)
            {
                Debug.Log(i);
                destructables[i].GetComponent<Demolishable>().Demolish(doFading, rigidBodyMaxLifetime, fadeTime, explosionForce, explosionRadius, upwardsModifier, mode);
            }

            i++;
            
            yield return new WaitForSeconds(Random.Range(0.0f, 0.5f));
        }
    }
}
