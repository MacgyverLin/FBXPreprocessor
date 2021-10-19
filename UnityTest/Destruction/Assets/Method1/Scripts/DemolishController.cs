﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DemolishController : MonoBehaviour
{
    public List<GameObject> refGameObjects;
    public float timeInterval = 0.7f;
    public int count = 80;
    
    public List<Demolishable> demolishables = new List<Demolishable>();

    public bool useExplosionPosition = false;
    public bool doFading = true;
    public float rigidBodyMaxLifetime = 4.0f;
    public float fadeTime = 1.0f;
    public float explosionForce = 100.0f;
    public float explosionRadius = 10.0f;
    public Vector3 explosionPosition = Vector3.zero;
    public float upwardsModifier = 10.0f;
    public ForceMode mode = ForceMode.Force;

    private void Start()
    {
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.M))
        {
            AddMoreDemolishables();
        }
        if (Input.GetKeyDown(KeyCode.Space))
        {
            Demolish(useExplosionPosition, doFading, rigidBodyMaxLifetime, fadeTime, explosionForce, explosionPosition, explosionRadius, upwardsModifier, mode);
        }
    }

    void AddMoreDemolishables()
    {
        for (int i = 0; i < count; i++)
        {
            float randomAngle = Random.Range(-10.0f, 10.0f);
            float randomX = Random.Range(-0.2f, 0.2f);
            float randomZ = Random.Range(-0.2f, 0.2f);
            float idx = Random.Range(0, refGameObjects.Count);
            if ((i % 2) == 0)
            {
                Quaternion q = new Quaternion();
                q.eulerAngles = new Vector3(0, randomAngle, 0);
                demolishables.Add(GameObject.Instantiate(refGameObjects[(int)idx], new Vector3(i / 2 * 6.5f + randomX - 70, 0, randomZ), q).GetComponent<Demolishable>());
            }
            else
            {
                Quaternion q = new Quaternion();
                q.eulerAngles = new Vector3(0, -180 + randomAngle, 0);
                demolishables.Add(GameObject.Instantiate(refGameObjects[(int)idx], new Vector3(i / 2 * 6.5f + randomX - 70, 0, 30 + randomZ), q).GetComponent<Demolishable>());
            }
        }
    }

    public void Demolish(bool useExplosionPosition, bool doFading, float rigidBodyMaxLifetime, float fadeTime, float explosionForce, Vector3 explosionPosition, float explosionRadius, float upwardsModifier, ForceMode mode)
    {
        StartCoroutine(DemolishCoroutine(useExplosionPosition, doFading, rigidBodyMaxLifetime, fadeTime, explosionForce, explosionPosition, explosionRadius, upwardsModifier, mode));
    }

    private IEnumerator DemolishCoroutine(bool useExplosionPosition, bool doFading, float rigidBodyMaxLifetime, float fadeTime, float explosionForce, Vector3 explosionPosition, float explosionRadius, float upwardsModifier, ForceMode mode)
    {
        int i = demolishables.Count - 1;

        while (i >= 0)
        {
            if (demolishables[i].isActiveAndEnabled)
            {
                Debug.Log(i);

                if (useExplosionPosition)
                    demolishables[i].GetComponent<Demolishable>().Demolish(doFading, rigidBodyMaxLifetime, fadeTime, explosionForce, explosionPosition, explosionRadius, upwardsModifier, mode);
                else
                    demolishables[i].GetComponent<Demolishable>().Demolish(doFading, rigidBodyMaxLifetime, fadeTime, explosionForce, demolishables[i].transform.position, explosionRadius, upwardsModifier, mode);
            }

            i--;

            yield return new WaitForSeconds(Random.Range(timeInterval * 0.3f, timeInterval));
        }
    }
}
