using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Destructable1 : MonoBehaviour
{
    public Material crossSectionMaterial;

    // Start is void called before the first frame update
    void Start()
    {
        Init();
    }

    // Update is called once per frame
    void Update()
    {
    }

    private void Init()
    {
        // enable Parent
        this.gameObject.SetActive(true);
        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        meshRenderer.enabled = true;

        // init child
        Material[] materials = new Material[meshRenderer.materials.Length+1];
        for(int i=0; i<meshRenderer.materials.Length+1; i++)
        {
            if(i==meshRenderer.materials.Length)
                materials[i] = crossSectionMaterial;
            else
                materials[i] = meshRenderer.materials[i];
        }

        for (int i = 0; i < transform.childCount; i++)
        {
            // Debug.Log(transform.GetChild(i).gameObject.name);
            InitChild(transform.GetChild(i).gameObject, materials);
        }
    }

    private void InitChild(GameObject child, Material[] materials)
    {
        // disable child
        child.SetActive(false);
        MeshRenderer meshRenderer = child.GetComponent<MeshRenderer>();
        meshRenderer.enabled = false;

        // uses parent material and cross section
        meshRenderer.materials = materials;

        // child use physics
        Rigidbody rigidbody = child.AddComponent<Rigidbody>();
        MeshCollider meshCollider = child.AddComponent<MeshCollider>();
        meshCollider.convex = true;
    }

    public void Destruct(bool doFading, float rigidBodyMaxLifetime, float fadeTime, float explosionForce, float explosionRadius, float upwardsModifier = 0.0f, ForceMode mode = ForceMode.Force)
    {
        StartCoroutine(DestructCoroutine(doFading, rigidBodyMaxLifetime, fadeTime, explosionForce, explosionRadius, upwardsModifier, mode));
    }

    private IEnumerator DestructCoroutine(bool doFading, float rigidBodyMaxLifetime, float fadeTime, float explosionForce, float explosionRadius, float upwardsModifier, ForceMode mode)
    {
        BeginDestruct(explosionForce, explosionRadius, upwardsModifier, mode);

        // yield return WaitAllRigidBodySleptOrTimeOut(Random.Range((rigidBodyMaxLifetime - 1.0f)/2, rigidBodyMaxLifetime - 1.0f));
        yield return WaitAllRigidBodySleptOrTimeOut(rigidBodyMaxLifetime - 1.0f);

        yield return new WaitForSeconds(1.0f);

        if (doFading)
        {
            yield return FadeChild(Random.Range(1.0f, fadeTime));

            yield return HideChild();
        }
    }

    public void BeginDestruct(float explosionForce, float explosionRadius, float upwardsModifier = 0.0f, ForceMode mode = ForceMode.Force)
    {
        // make sure parent, children are active
        this.gameObject.SetActive(true);

        // switch off rendering parent
        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        meshRenderer.enabled = false;

        for (int i = 0; i < transform.childCount; i++)
            DestructChild(transform.GetChild(i).gameObject, this.gameObject.transform.position, explosionForce, explosionRadius, upwardsModifier = 0.0f, mode);
    }

    private void DestructChild(GameObject child, Vector3 explosionPosition, float explosionForce, float explosionRadius, float upwardsModifier, ForceMode mode)
    {
        // make sure child is active
        child.SetActive(true);

        // switch on render child
        MeshRenderer meshRenderer = child.GetComponent<MeshRenderer>();
        meshRenderer.enabled = true;

        // do some physics
        Rigidbody rigidbody = child.GetComponent<Rigidbody>();
        Vector3 torque = new Vector3(Random.Range(-1.0f, 1.0f), Random.Range(-1.0f, 1.0f), Random.Range(-1.0f, 1.0f)) * explosionForce * 100;
        rigidbody.AddTorque(torque, mode);
        rigidbody.AddExplosionForce(explosionForce, explosionPosition, explosionRadius, upwardsModifier, mode);
    }

    IEnumerator WaitAllRigidBodySleptOrTimeOut(float timeout)
    {
        float startTime = Time.time;

        // can cache component?
        List<Rigidbody> rigidbodies = new List<Rigidbody>();
        for(int i = 0; i < transform.childCount; i++)
            rigidbodies.Add(transform.GetChild(i).GetComponent<Rigidbody>());

        bool done = false;
        while (!done && ((Time.time - startTime) < timeout))
        {
            done = true;
            foreach (var rb in rigidbodies)
            {
                if (!rb.IsSleeping())
                {
                    done = false;
                    break;
                }
            }
            
            yield return null;
        }
    }

    IEnumerator FadeChild(float timeout)
    {
        // can cache component?
        List<MeshRenderer> meshRenderers = new List<MeshRenderer>();
        for (int i = 0; i < transform.childCount; i++)
            meshRenderers.Add(transform.GetChild(i).GetComponent<MeshRenderer>());

        float startTime = Time.time;
        while ( (Time.time - startTime) < timeout)
        {
            float alpha = 1.0f - ((Time.time - startTime) / timeout);
            if (alpha < 0.0f)
                yield break;

            foreach (var meshRenderer in meshRenderers)
            {
                // ChangeAlpha(meshRenderer.materials, "_Alpha", alpha);

                foreach (var material in meshRenderer.materials)
                {
                    material.SetFloat("_Alpha", alpha);
                }
            }

            yield return null;
        }
    }

    public void ChangeAlpha(Material[] materials, string name, float alpha)
    {
        foreach (var material in materials)
        {
            material.SetFloat("_Alpha", alpha);
            material.SetInt("MySrcMode", (int)UnityEngine.Rendering.BlendMode.SrcAlpha);
            material.SetInt("MyDstMode", (int)UnityEngine.Rendering.BlendMode.Zero);
        }
    }

    IEnumerator HideChild()
    {
        List<MeshRenderer> meshRenderers = new List<MeshRenderer>();
        for (int i = 0; i < transform.childCount; i++)
            meshRenderers.Add(transform.GetChild(i).GetComponent<MeshRenderer>());

        foreach (var meshRenderer in meshRenderers)
        {
            meshRenderer.enabled = false;
        }

        yield return null;
    }
}