using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Destruction : MonoBehaviour
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
        this.gameObject.SetActive(true);
        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        meshRenderer.enabled = true;

        for (int i = 0; i < transform.childCount; i++)
        {
            // Debug.Log(transform.GetChild(i).gameObject.name);
            InitChild(transform.GetChild(i).gameObject, meshRenderer.materials);
        }
    }

    private void InitChild(GameObject child, Material[] materials)
    {
        child.SetActive(false);
        MeshRenderer meshRenderer = child.GetComponent<MeshRenderer>();
        meshRenderer.enabled = false;

        meshRenderer.materials = materials;

        Rigidbody rigidbody = child.AddComponent<Rigidbody>();

        MeshCollider meshCollider = child.AddComponent<MeshCollider>();
        meshCollider.convex = true;
    }

    public void Destruct(bool doFading, float timeout, float explosionForce, float explosionRadius, float upwardsModifier = 0.0f, ForceMode mode = ForceMode.Force)
    {
        StartCoroutine(DestructionCoroutine(doFading, timeout, explosionForce, explosionRadius, upwardsModifier, mode));
    }

    private IEnumerator DestructionCoroutine(bool doFading, float timeout, float explosionForce, float explosionRadius, float upwardsModifier, ForceMode mode)
    {
        BeginDestruct(explosionForce, explosionRadius, upwardsModifier, mode);

        yield return WaitAllRigidBodySleptOrTimeOut(timeout);

        if (doFading)
        {
            yield return FadeChild();

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
        Vector3 torque = new Vector3(Random.RandomRange(-1.0f, 1.0f), Random.RandomRange(-1.0f, 1.0f), Random.RandomRange(-1.0f, 1.0f)) * explosionForce * 100;
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

    public virtual float ChangeAlpha(Material material)
    {
        return 0.0f;
    }

    IEnumerator FadeChild()
    {
        yield return null;
    }

    IEnumerator HideChild()
    {
        // can cache component?
        List<MeshRenderer> meshRenderers = new List<MeshRenderer>();
        for (int i = 0; i < transform.childCount; i++)
            meshRenderers.Add(transform.GetChild(i).GetComponent<MeshRenderer>());

        foreach (var meshRenderer in meshRenderers)
        {
            // Debug.Log(transform.GetChild(i).gameObject.name);
            meshRenderer.enabled = false;
        }

        yield return null;
    }
}