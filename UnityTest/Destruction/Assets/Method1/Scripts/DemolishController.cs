using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DemolishController : MonoBehaviour
{
    public List<GameObject> gameObjs;

    public float timeInterval = 0.7f;
    public int count = 80;

    public List<Demolishable> demolishables = new List<Demolishable>();

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
        if (Input.GetKeyDown(KeyCode.M))
        {
            AddDemolishables();
        }
        if (Input.GetKeyDown(KeyCode.Space))
        {
            Demolish(doFading, rigidBodyMaxLifetime, fadeTime, explosionForce, explosionRadius, upwardsModifier, mode);
        }
    }

    void AddDemolishables()
    {
        for (int i = 0; i < count; i++)
        {
            //if (testGameObj != null)
            {
                float randomAngle = Random.Range(-10.0f, 10.0f);
                float randomX = Random.Range(-0.2f, 0.2f);
                float randomZ = Random.Range(-0.2f, 0.2f);
                float idx = Random.Range(0, gameObjs.Count);
                Debug.Log(idx);
                if ((i % 2)==0)
                {
                    Quaternion q = new Quaternion();
                    q.eulerAngles = new Vector3(0, randomAngle, 0);
                    demolishables.Add(GameObject.Instantiate(gameObjs[(int)idx], new Vector3(i/2 * 4.5f + randomX, 0, randomZ), q).GetComponent<Demolishable>());
                }
                else
                {
                    Quaternion q = new Quaternion();
                    q.eulerAngles = new Vector3(0, -180 + randomAngle, 0);
                    demolishables.Add(GameObject.Instantiate(gameObjs[(int)idx], new Vector3(i / 2 * 4.5f + randomX, 0, 10 + randomZ), q).GetComponent<Demolishable>());
                }
            }
        }
    }

    public void Demolish(bool doFading, float rigidBodyMaxLifetime, float fadeTime, float explosionForce, float explosionRadius, float upwardsModifier = 0.0f, ForceMode mode = ForceMode.Force)
    {
        StartCoroutine(DemolishCoroutine(doFading, rigidBodyMaxLifetime, fadeTime, explosionForce, explosionRadius, upwardsModifier, mode));
    }

    private IEnumerator DemolishCoroutine(bool doFading, float rigidBodyMaxLifetime, float fadeTime, float explosionForce, float explosionRadius, float upwardsModifier, ForceMode mode)
    {
        int i = 0;

        while (i < demolishables.Count)
        {
            if (demolishables[i].isActiveAndEnabled)
            {
                Debug.Log(i);
                demolishables[i].GetComponent<Demolishable>().Demolish(doFading, rigidBodyMaxLifetime, fadeTime, explosionForce, explosionRadius, upwardsModifier, mode);
            }

            i++;
            
            yield return new WaitForSeconds(Random.Range(timeInterval * 0.3f, timeInterval));
        }
    }
}
