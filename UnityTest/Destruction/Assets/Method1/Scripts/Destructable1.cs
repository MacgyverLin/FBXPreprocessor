using System.Collections;
using System.Collections.Generic;
using UnityEngine;

class MiniRigidbody
{
    private Vector3 linearPosition = new Vector3();
    private Vector3 linearVelocity = new Vector3();
    private Vector3 angularPosition = new Vector3();
    private Vector3 angularVelocity = new Vector3();
    private Vector3 localScale = new Vector3();
    bool sleeping;

    public MiniRigidbody()
    {
    }

    public void Init(Vector3 linearPosition, Vector3 linearVelocity, Vector3 angularPosition, Vector3 angularVelocity, Vector3 localScale)
    {
        this.sleeping = false;
        this.linearPosition = linearPosition;
        this.linearVelocity = linearVelocity;
        this.angularPosition = angularPosition;
        this.angularVelocity = angularVelocity;
        this.localScale = localScale;
    }

    public void Update(Vector3 linearAcc, Vector3 angularAcc, float linearDrag, float angularDrag, float dt)
    {
        if (sleeping)
            return;

        RaycastHit hitInfo = new RaycastHit();
        bool hit = Physics.Raycast(this.linearPosition, this.linearVelocity, out hitInfo, this.linearVelocity.magnitude * dt * 2);
        if (hit)
        {
            linearDrag = 0.1f;
            angularDrag = 0.1f;
            linearAcc = Vector3.zero;
            angularAcc = Vector3.zero;

            ///////////////////////////////////////
            this.linearVelocity -= this.linearVelocity * linearDrag;
            this.linearPosition += this.linearVelocity * dt;

            ///////////////////////////////////////
            this.angularVelocity -= this.angularVelocity * angularDrag;
            this.angularPosition += this.angularVelocity * dt;

            this.linearPosition.y = hitInfo.point.y;

            sleeping = true;
        }
        else
        {
            ///////////////////////////////////////
            this.linearVelocity -= this.linearVelocity * linearDrag;
            this.linearVelocity += linearAcc * dt;
            this.linearPosition += this.linearVelocity * dt;

            ///////////////////////////////////////
            this.angularVelocity -= this.angularVelocity * angularDrag;
            this.angularVelocity += angularAcc * dt;
            this.angularPosition += this.angularVelocity * dt;
        }
    }

    public bool IsSleeping()
    {
        return sleeping;
    }

    public Matrix4x4 GetTransform()
    {
        Matrix4x4 result = new Matrix4x4();
        Quaternion q = new Quaternion();
        q.eulerAngles = angularPosition;

        result.SetTRS(this.linearPosition, q, localScale);

        return result;
    }
};

public class Destructable1 : MonoBehaviour
{
    private Vector3 linearAcc = new Vector3(0.0f, -9.8f, 0.0f);
    private Vector3 angularAcc = new Vector3(0.0f, 0.0f, 0.0f);
    private float linearDrag = 0.01f;
    private float angularDrag = 0.01f;
    private MiniRigidbody[] miniRigidbodies = new MiniRigidbody[16];
    private Matrix4x4[] transforms = new Matrix4x4[16];

    // Start is void called before the first frame update
    void Start()
    {
        Init();
    }

    void Reset()
    {
        Init();
    }

    // Update is called once per frame
    void Update()
    {
    }

    private void Init(float explosionForce = 0, float explosionRadius = 0, float upwardsModifier = 0.0f, ForceMode mode = ForceMode.Force)
    {
        float linearSpeed = 10.0f * explosionForce;
        float angularSpeed = 360.0f * explosionForce;

        for (int i = 0; i < miniRigidbodies.Length; i++)
        {
            miniRigidbodies[i] = new MiniRigidbody();
            miniRigidbodies[i].Init
            (
                this.transform.position,
                new Vector3(Random.Range(-linearSpeed, linearSpeed), Random.Range(0.0f, linearSpeed * upwardsModifier), Random.Range(-linearSpeed, linearSpeed)),
                this.transform.rotation.eulerAngles,
                new Vector3(Random.Range(-angularSpeed, angularSpeed), Random.Range(-angularSpeed, angularSpeed), Random.Range(-angularSpeed, angularSpeed)),
                this.transform.localScale
            );
        }

        for (int i = 0; i < transforms.Length; i++)
        {
            transforms[i] = miniRigidbodies[i].GetTransform();
        }

        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        for (int i = 0; i < meshRenderer.materials.Length; i++)
        {
            meshRenderer.materials[i].SetFloat("_ShowCrossSection", 0.0f);
            meshRenderer.materials[i].SetMatrixArray("_Transforms", transforms);
        }
    }

    void UpdatePhysics()
    {
        for (int i = 0; i < miniRigidbodies.Length; i++)
        {
            miniRigidbodies[i].Update(linearAcc, angularAcc, linearDrag, angularDrag, Time.deltaTime);
            transforms[i] = miniRigidbodies[i].GetTransform();
        }

        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        for (int i = 0; i < meshRenderer.materials.Length; i++)
        {
            meshRenderer.materials[i].SetFloat("_ShowCrossSection", 1.0f);
            meshRenderer.materials[i].SetMatrixArray("_Transforms", transforms);
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
        yield return WaitAllRigidBodySleptOrTimeOut(rigidBodyMaxLifetime);

        yield return new WaitForSeconds(1.0f);

        if (doFading)
        {
            yield return FadeChild(fadeTime);

            yield return HideChild();
        }
    }

    public void BeginDestruct(float explosionForce, float explosionRadius, float upwardsModifier = 0.0f, ForceMode mode = ForceMode.Force)
    {
        // make sure visible
        this.gameObject.SetActive(true);
        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        meshRenderer.enabled = true;

        Init(explosionForce, explosionRadius, upwardsModifier, mode);
    }

    IEnumerator WaitAllRigidBodySleptOrTimeOut(float timeout)
    {
        float startTime = Time.time;

        bool done = false;
        while (!done/* && (Time.time-startTime<timeout)*/)
        {
            UpdatePhysics();

            done = true;
            foreach (var rb in miniRigidbodies)
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
        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();

        float startTime = Time.time;
        while ((Time.time - startTime) < timeout)
        {
            float alpha = 1.0f - ((Time.time - startTime) / timeout);
            if (alpha < 0.0f)
                yield break;

            for (int i = 0; i < meshRenderer.materials.Length; i++)
            {
                meshRenderer.materials[i].SetFloat("_Alpha", alpha);
            }

            yield return null;
        }

        yield return null;
    }


    IEnumerator HideChild()
    {
        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        meshRenderer.enabled = false;

        yield return null;
    }
}