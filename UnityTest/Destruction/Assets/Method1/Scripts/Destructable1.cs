using System.Collections;
using System.Collections.Generic;
using UnityEngine;

class MiniPhysics
{
    private Vector3 linearPosition = new Vector3();
    private Vector3 linearVelocity = new Vector3();
    private Vector3 angularPosition = new Vector3();
    private Vector3 angularVelocity = new Vector3();

    public MiniPhysics()
    {
    }

    public void Init(Vector3 linearPosition, Vector3 linearVelocity, Vector3 angularPosition, Vector3 angularVelocity)
    {
        this.linearPosition = linearPosition;
        this.linearVelocity = linearVelocity;
        this.angularPosition = angularPosition;
        this.angularVelocity = angularVelocity;
    }

    public void Update(Vector3 linearAcc, Vector3 angularAcc, float linearDrag, float angularDrag, float dt)
    {
        ///////////////////////////////////////
        this.linearVelocity -= this.linearVelocity * dt;
        this.linearVelocity += linearAcc * dt;
        this.linearPosition += this.linearVelocity * dt;

        ///////////////////////////////////////
        this.angularVelocity -= this.angularVelocity * angularDrag * dt;
        this.angularVelocity += angularAcc * dt;
        this.angularPosition += this.angularVelocity * dt;

        RaycastHit hitInfo = new RaycastHit();
        if (Physics.Raycast(this.linearPosition, this.linearVelocity, out hitInfo))
        {
            // this.linearVelocity -= 2.0f * Vector3.Dot(this.linearVelocity, hitInfo.normal) * hitInfo.normal;
            //this.linearPosition = hitInfo.point + this.linearVelocity * dt  * 0.5f;
            this.linearPosition.y = 0.0f;
        }
    }

    public Matrix4x4 GetTransform()
    {
        Matrix4x4 result = new Matrix4x4();
        Quaternion q = new Quaternion();
        q.eulerAngles = angularPosition;

        result.SetTRS(this.linearPosition, q, Vector3.one);

        return result;
    }
};

public class Destructable1 : MonoBehaviour
{
    // public Material crossSectionMaterial;
    public bool showCrossSection = false;

    private Vector3 linearAcc = new Vector3(0.0f, -9.8f, 0.0f);
    private Vector3 angularAcc = new Vector3(0.0f, 0.0f, 0.0f);
    private float linearDrag = 0.01f;
    private float angularDrag = 0.01f;
    private MiniPhysics[] miniPhysics = new MiniPhysics[16];
    private Matrix4x4[] transforms = new Matrix4x4[16];

    // Start is void called before the first frame update
    void Start()
    {
        Init();
    }

    void Reset()
    {
        Init();
        UpdateCrossSection();
    }

    // Update is called once per frame
    void Update()
    {
        UpdateCrossSection();
    }

    private void Init()
    {
        float V1 = 20.0f;
        float V2 = 10.0f;

        float AV1 = 180.0f * 1.0f;

        for (int i = 0; i < miniPhysics.Length; i++)
        {
            miniPhysics[i] = new MiniPhysics();
            miniPhysics[i].Init
            (
                this.transform.position,
                new Vector3(Random.Range(-V1, V1), Random.Range(0.0f, V2), Random.Range(-V1, V1)),
                this.transform.rotation.eulerAngles,
                new Vector3(Random.Range(-AV1, AV1), Random.Range(-AV1, AV1), Random.Range(-AV1, AV1))
            );
        }

        for (int i = 0; i < transforms.Length; i++)
        {
            transforms[i] = miniPhysics[i].GetTransform();
        }
    }

    void UpdateCrossSection()
    {
        if (showCrossSection)
        {
            UpdatePhysics();
        }
        else
        {
            Init();
        }

        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        for (int i = 0; i < meshRenderer.materials.Length; i++)
        {
            meshRenderer.materials[i].SetFloat("_ShowCrossSection", showCrossSection ? 1.0f : 0.0f);
            meshRenderer.materials[i].SetMatrixArray("_Transforms", transforms);
        }
    }

    void UpdatePhysics()
    {
        for (int i = 0; i < miniPhysics.Length; i++)
        {
            miniPhysics[i].Update(linearAcc, angularAcc, linearDrag, angularDrag, Time.deltaTime);
            transforms[i] = miniPhysics[i].GetTransform();
        }
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