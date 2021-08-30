using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Destructable1 : MonoBehaviour
{
    public Material crossSectionMaterial;
    public bool showCrossSection = false;

    // Start is void called before the first frame update
    void Start()
    {
        Init();
    }

    // Update is called once per frame
    void Update()
    {
        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();

        Matrix4x4[] transforms = new Matrix4x4[3];

        Quaternion rotate0 = Quaternion.Euler(0, 0, 360.0f * Time.time / 20.0f);
        Vector3 translate0 = new Vector3(-5.0f * Mathf.Abs(Mathf.Sin(2.0f * Mathf.PI * Time.time / 20.0f)), 0.0f, 0.0f);
        transforms[0] = Matrix4x4.TRS(translate0, rotate0, Vector3.one);

        Quaternion rotate1 = Quaternion.Euler(0, 0, 360.0f * Time.time / 20.0f);
        Vector3 translate1 = new Vector3(5.0f * Mathf.Abs(Mathf.Sin(2.0f * Mathf.PI * Time.time / 20.0f)), 0.0f, 0.0f);
        transforms[1] = Matrix4x4.TRS(translate1, rotate1, Vector3.one);

        Quaternion rotate2 = Quaternion.Euler(0, 0, 360.0f * Time.time / 20.0f);
        Vector3 translate2 = new Vector3(0.0f, 5.0f * Mathf.Abs(Mathf.Sin(2.0f * Mathf.PI * Time.time / 20.0f)), 0.0f);
        transforms[2] = Matrix4x4.TRS(translate2, rotate2, Vector3.one);

        for (int i = 0; i < meshRenderer.materials.Length; i++)
        {
            meshRenderer.materials[i].SetFloat("_ShowCrossSection", showCrossSection ? 1.0f : 0.0f);
            meshRenderer.materials[i].SetMatrixArray("_Transforms", transforms);
        }
    }

    private void Init()
    {
        // make sure visible
        this.gameObject.SetActive(true);
        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        meshRenderer.enabled = true;

        Material[] materials = meshRenderer.materials;
        materials[materials.Length - 1] = crossSectionMaterial;
        meshRenderer.materials = materials;
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
        // make sure visible
        this.gameObject.SetActive(true);
        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        meshRenderer.enabled = true;
    }

    IEnumerator WaitAllRigidBodySleptOrTimeOut(float timeout)
    {
        float startTime = Time.time;

        /*
        // can cache component?
        List<Rigidbody> rigidbodies = new List<Rigidbody>();
        for (int i = 0; i < transform.childCount; i++)
            rigidbodies.Add(transform.GetChild(i).GetComponent<Rigidbody>());
        */

        bool done = false;
        while (!done && ((Time.time - startTime) < timeout))
        {
            /*
            done = true;
            foreach (var rb in rigidbodies)
            {
                if (!rb.IsSleeping())
                {
                    done = false;
                    break;
                }
            }
            */

            yield return null;
        }
    }

    IEnumerator FadeChild(float timeout)
    {
        /*
        // can cache component?
        List<MeshRenderer> meshRenderers = new List<MeshRenderer>();
        for (int i = 0; i < transform.childCount; i++)
            meshRenderers.Add(transform.GetChild(i).GetComponent<MeshRenderer>());

        float startTime = Time.time;
        while ((Time.time - startTime) < timeout)
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
        */
        yield return null;
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
        /*
        List<MeshRenderer> meshRenderers = new List<MeshRenderer>();
        for (int i = 0; i < transform.childCount; i++)
            meshRenderers.Add(transform.GetChild(i).GetComponent<MeshRenderer>());

        foreach (var meshRenderer in meshRenderers)
        {
            meshRenderer.enabled = false;
        }
        */

        yield return null;
    }


    /*
    private void InitChild(GameObject child)
    {
        // disable child
        child.SetActive(false);
        MeshRenderer meshRenderer = child.GetComponent<MeshRenderer>();
        meshRenderer.enabled = false;

        // last material must be crossSection Material
        Material[] matArray = meshRenderer.materials;
        matArray[matArray.Length - 1] = crossSectionMaterial;
        meshRenderer.materials = matArray;

        // child use physics
        Rigidbody rigidbody = child.AddComponent<Rigidbody>();
        MeshCollider meshCollider = child.AddComponent<MeshCollider>();
        meshCollider.convex = true;
    }
    */

}