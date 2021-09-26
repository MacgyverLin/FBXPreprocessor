using System.Collections;
using System.Collections.Generic;
using UnityEngine;

class PhysicsBase
{
    protected Matrix4x4 transform = Matrix4x4.identity;
    protected Vector4 translation = new Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    protected Vector4 rotation = new Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    protected Vector4 scale = new Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    protected bool sleeping = false;

    public PhysicsBase()
    {
    }

    virtual public void Update()
    {
    }

    public bool IsSleeping()
    {
        return sleeping;
    }

    public Matrix4x4 GetTransform()
    {
        return transform;
    }

    public Vector4 GetTranslation()
    {
        return translation;
    }

    public Vector4 GetRotation()
    {
        return rotation;
    }

    public Vector4 GetScale()
    {
        return new Vector4(scale.x, scale.y, scale.z, 1.0f);
    }
};


class SimplePhysics : PhysicsBase
{
    protected Vector3 linearPosition = new Vector3(0.0f, 0.0f, 0.0f);
    protected Vector3 linearVelocity = new Vector3(0.0f, 0.0f, 0.0f);
    protected Vector3 linearAcc = new Vector3(0.0f, 0.0f, 0.0f);
    protected float linearDrag = 0.0f;
    
    protected Vector3 angularPosition = new Vector3(0.0f, 0.0f, 0.0f);
    protected Vector3 angularVelocity = new Vector3(0.0f, 0.0f, 0.0f);
    protected Vector3 angularAcc = new Vector3(0.0f, 0.0f, 0.0f);
    protected float angularDrag = 0.0f;

    public SimplePhysics()
    {
    }

    public void Init(GameObject gameobject, Vector3 linearPosition, Vector3 linearVelocity, Vector3 linearAcc, float linearDrag,
                     Vector3 angularPosition, Vector3 angularVelocity, Vector3 angularAcc, float angularDrag,
                     Vector3 scale)
    {
        this.sleeping = false;
        this.linearPosition = linearPosition;
        this.linearVelocity = linearVelocity;
        this.linearAcc = linearAcc;
        this.linearDrag = linearDrag;

        this.angularPosition = angularPosition;
        this.angularVelocity = angularVelocity;
        this.angularAcc = angularAcc;
        this.angularDrag = angularDrag;

        this.scale = new Vector4(scale.x, scale.y, scale.z, 1.0f);
    }

    public override void Update()
    {
        if (sleeping)
            return;

        float dt = Time.deltaTime;

        RaycastHit hitInfo = new RaycastHit();
        bool hit = Physics.Raycast(this.linearPosition, this.linearVelocity, out hitInfo, this.linearVelocity.magnitude * dt);
        if (hit)
        {
            float fraction = hitInfo.distance / this.linearVelocity.magnitude;
            // Debug.Log(string.Format("Time:{0}, Distance:{1}, Mag:{2} ", fraction, hitInfo.distance, this.linearVelocity.magnitude));

            ///////////////////////////////////////
            this.linearPosition += this.linearVelocity * dt * fraction;
            this.linearVelocity -= this.linearVelocity * this.linearDrag;

            ///////////////////////////////////////
            this.angularPosition += this.angularVelocity * dt * fraction;
            this.angularVelocity -= this.angularVelocity * this.angularDrag * dt;

            this.linearPosition = hitInfo.point;
            //this.angularPosition = new Vector3(0.0f, 0.0f, 0.0f);// hitInfo.normal;

            sleeping = true;
        }
        else
        {
            ///////////////////////////////////////
            this.linearPosition += this.linearVelocity * dt;
            this.linearVelocity -= this.linearVelocity * this.linearDrag * dt;
            this.linearVelocity += this.linearAcc * dt;

            ///////////////////////////////////////
            this.angularPosition += this.angularVelocity * dt;
            this.angularVelocity -= this.angularVelocity * this.angularDrag * dt;
            this.angularVelocity += this.angularAcc * dt;
        }

        if (this.linearPosition.y < 1)
            sleeping = true;
        //////////////////////////////
        translation = this.linearPosition;

        Quaternion q = new Quaternion();
        q.eulerAngles = angularPosition;
        rotation = new Vector4(q.x, q.y, q.z, q.w);

        transform.SetTRS(this.linearPosition, q, scale);
    }
};

class RigidbodyPhysics : PhysicsBase
{
    public RigidbodyPhysics()
    {
    }

    public void Init(GameObject gameobject, Vector3 linearPosition, Vector3 linearVelocity, Vector3 linearAcc, float linearDrag,
                     Vector3 angularPosition, Vector3 angularVelocity, Vector3 angularAcc, float angularDrag,
                     Vector3 scale)
    {
        this.sleeping = false;


    }

    public override void Update()
    {
        float dt = Time.deltaTime;
        if (sleeping)
            return;
    
    }
};

public class Destructable1 : MonoBehaviour
{
    private SimplePhysics[] physics = new SimplePhysics[16];

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
        //MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        //MeshFilter meshFilter = meshRenderer.GetComponent<MeshFilter>();
        //meshFilter.mesh.bounds = new Bounds(Vector3.zero, Vector3.one * 100.0f);
    }

    private void Init(float explosionForce = 0, float explosionRadius = 0, float upwardsModifier = 0.0f, ForceMode mode = ForceMode.Force)
    {
        float linearSpeed = 10.0f * explosionForce;
        float angularSpeed = 360.0f * explosionForce;

        for (int i = 0; i < physics.Length; i++)
        {
            physics[i] = new SimplePhysics();
            physics[i].Init
            (
                this.gameObject,
                this.transform.position,
                new Vector3(Random.Range(-linearSpeed, linearSpeed), Random.Range(0.0f, linearSpeed * upwardsModifier), Random.Range(-linearSpeed, linearSpeed)),
                new Vector3(0.0f, -9.8f, 0.0f),
                0.01f,
                this.transform.rotation.eulerAngles,
                new Vector3(Random.Range(-angularSpeed, angularSpeed), Random.Range(-angularSpeed, angularSpeed), Random.Range(-angularSpeed, angularSpeed)),
                new Vector3(0.0f, 0.0f, 0.0f),
                0.01f,
                this.transform.localScale
            );
        }

        Matrix4x4[] transforms = new Matrix4x4[16];
        Vector4[] translations = new Vector4[16];
        Vector4[] rotations = new Vector4[16];
        for (int i = 0; i < transforms.Length; i++)
        {
            transforms[i] = physics[i].GetTransform();
            translations[i] = physics[i].GetTranslation();
            rotations[i] = physics[i].GetRotation();
        }

        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        for (int i = 0; i < meshRenderer.materials.Length; i++)
        {
            meshRenderer.materials[i].SetFloat("_IsDestructed", 0.0f);
            meshRenderer.materials[i].SetMatrixArray("_Transforms", transforms);

            meshRenderer.materials[i].SetVectorArray("_Translate", translations);
            meshRenderer.materials[i].SetVectorArray("_Rotation", rotations);
        }
    }

    void UpdatePhysics()
    {
        Matrix4x4[] transforms = new Matrix4x4[16];
        Vector4[] translations = new Vector4[16];
        Vector4[] rotations = new Vector4[16];
        for (int i = 0; i < physics.Length; i++)
        {
            physics[i].Update();
            transforms[i] = physics[i].GetTransform();

            translations[i] = physics[i].GetTranslation();
            rotations[i] = physics[i].GetRotation();
        }

        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        for (int i = 0; i < meshRenderer.materials.Length; i++)
        {
            meshRenderer.materials[i].SetFloat("_IsDestructed", 1.0f);
            meshRenderer.materials[i].SetMatrixArray("_Transforms", transforms);

            meshRenderer.materials[i].SetVectorArray("_Translate", translations);
            meshRenderer.materials[i].SetVectorArray("_Rotation", rotations);
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
            foreach (var rb in physics)
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