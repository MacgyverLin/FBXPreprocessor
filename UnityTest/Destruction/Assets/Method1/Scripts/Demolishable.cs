using System.Collections;
using System.Collections.Generic;
using UnityEngine;

class PhysicsBase
{
    protected Matrix4x4 transform = Matrix4x4.identity;
    protected Vector4 scale = new Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    protected bool sleeping = false;

    protected FaceGroup faceGroup;

    public PhysicsBase()
    {
    }

    virtual public void Update(bool testPivot)
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

    public FaceGroup GetFaceGroup()
    {
        return faceGroup;
    }
};

class SimplePhysics : PhysicsBase
{
    protected Vector3 linearPosition = new Vector3(0.0f, 0.0f, 0.0f);
    protected Vector3 linearVelocity = new Vector3(0.0f, 0.0f, 0.0f);
    protected Vector3 linearAcc = new Vector3(0.0f, 0.0f, 0.0f);
    protected float linearDrag = 0.1f;

    protected Quaternion angularPosition = new Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
    protected Quaternion angularVelocity = new Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
    protected Quaternion angularAcc = new Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
    protected float angularDrag = 0.1f;

    protected float elasticity = 0.3f;
    protected float friction = 0.4f;

    public SimplePhysics(FaceGroup faceGroup)
    {
        this.faceGroup = faceGroup;
    }

    public void Init(GameObject gameobject,
                     Vector3 linearVelocity, Vector3 linearAcc, float linearDrag,
                     Quaternion angularVelocity, Quaternion angularAcc, float angularDrag,
                     Vector3 scale)
    {
        this.sleeping = false;
        this.linearPosition = gameobject.transform.TransformPoint(faceGroup.bound.center);
        this.linearVelocity = linearVelocity;
        this.linearAcc = linearAcc;
        this.linearDrag = linearDrag;

        this.angularPosition = gameobject.transform.rotation;
        this.angularVelocity = angularVelocity;
        this.angularAcc = angularAcc;
        this.angularDrag = angularDrag;

        this.scale = new Vector4(scale.x, scale.y, scale.z, 1.0f);
    }

    public void TestPivot(bool testPivot)
    {
        if (sleeping)
            return;

        //////////////////////////////
        Vector3 pivot = faceGroup.bound.center;

        Matrix4x4 m1 = Matrix4x4.identity;
        m1.SetTRS(-pivot, Quaternion.identity, Vector3.one);

        Matrix4x4 m2 = Matrix4x4.identity;
        m2.SetTRS(Vector3.zero, Quaternion.AngleAxis(30.0f, Vector3.right), Vector3.one);

        Matrix4x4 m3 = Matrix4x4.identity;
        m3.SetTRS(pivot, Quaternion.identity, Vector3.one);

        Matrix4x4 m4 = Matrix4x4.identity;
        m4.SetTRS(Vector3.zero, Quaternion.identity, scale);

        transform = m4 * m3 * m2 * m1;
    }

    Quaternion ScaledQuaternion(Quaternion quaternion, float scale)
    {
        return new Quaternion(quaternion.x * scale, quaternion.y * scale, quaternion.z * scale, quaternion.w * scale);
    }

    Quaternion SummedQuaternion(Quaternion quaternion_alpha, Quaternion quaternion_beta)
    {
        return new Quaternion(quaternion_alpha.x + quaternion_beta.x, quaternion_alpha.y + quaternion_beta.y, quaternion_alpha.z + quaternion_beta.z, quaternion_alpha.w + quaternion_beta.w);
    }

    Quaternion AddQuaternionDerivative(Quaternion q, Quaternion dq, float dt)
    {
        return SummedQuaternion(q, ScaledQuaternion(dq, dt / 2) * q).normalized;
    }

    public void UpdateDynamics()
    {
        if (sleeping)
            return;

        float dt = Time.deltaTime;
        if (Input.GetKey(KeyCode.B))
            dt = dt * 0.05f;

        RaycastHit hitInfo = new RaycastHit();
        bool hit = Physics.SphereCast(this.linearPosition, 1.0f, this.linearVelocity.normalized, out hitInfo, this.linearVelocity.magnitude * dt);
        if (hit)
        {
            float fraction = hitInfo.distance / this.linearVelocity.magnitude;
            float dtFraction = dt * fraction;

            ///////////////////////////////////////
            // p
            this.linearPosition += dtFraction * this.linearVelocity;

            // drag
            this.linearVelocity -= (this.linearDrag) * this.linearVelocity;

            // acc
            this.linearVelocity += dtFraction * this.linearAcc;

            Vector3 vertVelocity = hitInfo.normal * Vector3.Dot(this.linearVelocity, hitInfo.normal);
            Vector3 horiVelocity = this.linearVelocity - vertVelocity;

            // bounce and friction
            this.linearVelocity -= (1.0f + elasticity) * vertVelocity;
            this.linearVelocity -= friction * horiVelocity;

            ///////////////////////////////////////
            // p
            this.angularPosition = AddQuaternionDerivative(this.angularPosition, this.angularVelocity, dtFraction * 10.0f);

            // drag
            this.angularVelocity = AddQuaternionDerivative(this.angularVelocity, this.angularVelocity, -this.angularDrag);

            // acc
            this.angularVelocity = AddQuaternionDerivative(this.angularVelocity, this.angularAcc, dtFraction * 10.0f);

            // bounce and friction
            this.angularVelocity = AddQuaternionDerivative(this.angularVelocity, this.angularVelocity, -friction);

            //sleeping = true;
        }
        else
        {
            ///////////////////////////////////////
            // p
            this.linearPosition += dt * this.linearVelocity ;

            // drag
            this.linearVelocity -= (this.linearDrag) * this.linearVelocity;

            // acc
            this.linearVelocity += dt * this.linearAcc;

            ///////////////////////////////////////
            // p
            this.angularPosition = AddQuaternionDerivative(this.angularPosition, this.angularVelocity, dt * 10.0f);

            // drag
            this.angularVelocity = AddQuaternionDerivative(this.angularVelocity, this.angularVelocity, -this.angularDrag);

            // acc
            this.angularVelocity = AddQuaternionDerivative(this.angularVelocity, this.angularAcc, dt * 10.0f);
        }

        //////////////////////////////
        Vector3 pivot = faceGroup.bound.center;

        Matrix4x4 m1 = Matrix4x4.identity;
        m1.SetTRS(-pivot, Quaternion.identity, Vector3.one);

        Matrix4x4 m2 = Matrix4x4.identity;
        m2.SetTRS(Vector3.zero, angularPosition, Vector3.one);

        //Matrix4x4 m3 = Matrix4x4.identity;
        //m3.SetTRS(pivot, Quaternion.identity, Vector3.one);

        Matrix4x4 m4 = Matrix4x4.identity;
        m4.SetTRS(this.linearPosition, Quaternion.identity, scale);

        transform = m4 * m2 * m1;
    }

    public override void Update(bool testPivot)
    {
        if (testPivot)
            TestPivot(testPivot);
        else
            UpdateDynamics();
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

    public override void Update(bool testPivot)
    {
        float dt = Time.deltaTime;
        if (sleeping)
            return;
    }
};

public class Demolishable : MonoBehaviour
{
    private SimplePhysics[] physics;
    public DemolishableData demolishableData;
    public bool testPivot = false;

    // Start is void called before the first frame update
    void Start()
    {
        Initialize();
    }

    void Reset()
    {
        Initialize();
    }

    // Update is called once per frame
    void Update()
    {
    }

    private void Initialize(float explosionForce = 0, float explosionRadius = 0, float upwardsModifier = 0.0f, ForceMode mode = ForceMode.Force)
    {
        //demolishableData = GameObject.Instantiate<DemolishableData>(Resources.Load<DemolishableData>(GetComponent<MeshFilter>().name));

        float linearSpeed = 10.0f * explosionForce;
        float angularSpeed = 360.0f * 30.0f * explosionForce;

        physics = new SimplePhysics[demolishableData.GetFaceGroupCount()];
        for (int i = 0; i < demolishableData.GetFaceGroupCount(); i++)
        {
            FaceGroup faceGroup = demolishableData.GetFaceGroup(i);

            Quaternion q = Quaternion.AngleAxis(Random.Range(-32, 32), new Vector3(Random.Range(-1, 1), Random.Range(-1, 1), Random.Range(-1, 1)).normalized);

            physics[i] = new SimplePhysics(faceGroup);
            physics[i].Init
            (
                this.gameObject,
                new Vector3(Random.Range(-linearSpeed, linearSpeed), Random.Range(0.0f, linearSpeed * upwardsModifier), Random.Range(-linearSpeed, linearSpeed)),
                new Vector3(0.0f, -9.8f, 0.0f),
                0.01f,
                Quaternion.AngleAxis(Random.Range(0, angularSpeed), new Vector3(Random.Range(-1, 1), Random.Range(-1, 1), Random.Range(-1, 1))),
                Quaternion.AngleAxis(0.0f, Vector3.right),
                0.01f,
                this.transform.localScale
            );
        }

        UpdatePhysics(false);
    }

    void UpdatePhysics(bool showCrossSection)
    {
        Matrix4x4[] transforms = new Matrix4x4[32];
        //Vector4[] translations = new Vector4[32];
        //Vector4[] rotations = new Vector4[32];
        //Vector4[] pivots = new Vector4[32];

        for (int i = 0; i < physics.Length; i++)
        {
            int groupID = demolishableData.GetFaceGroup(i).groupID;

            physics[i].Update(testPivot);
            transforms[groupID] = physics[i].GetTransform();

            //translations[groupID] = physics[i].GetTranslation();
            //rotations[groupID] = physics[i].GetRotation();
            //pivots[groupID] = physics[i].GetFaceGroup().bound.center;
        }

        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        for (int i = 0; i < meshRenderer.materials.Length; i++)
        {
            meshRenderer.materials[i].SetFloat("_IsDestructed", showCrossSection ? 1.0f : 0.0f);
            meshRenderer.materials[i].SetMatrixArray("_Transforms", transforms);

            //meshRenderer.materials[i].SetVectorArray("_Translate", translations);
            //meshRenderer.materials[i].SetVectorArray("_Rotation", rotations);
        }
    }

    /// ////////////////////////////////////////////////////////////////////////////////////
    public void Demolish(bool doFading, float rigidBodyMaxLifetime, float fadeTime, float explosionForce, float explosionRadius, float upwardsModifier = 0.0f, ForceMode mode = ForceMode.Force)
    {
        StartCoroutine(DemolishCoroutine(doFading, rigidBodyMaxLifetime, fadeTime, explosionForce, explosionRadius, upwardsModifier, mode));
    }

    private IEnumerator DemolishCoroutine(bool doFading, float rigidBodyMaxLifetime, float fadeTime, float explosionForce, float explosionRadius, float upwardsModifier, ForceMode mode)
    {
        BeginDemolish(explosionForce, explosionRadius, upwardsModifier, mode);

        // yield return WaitAllRigidBodySleptOrTimeOut(Random.Range((rigidBodyMaxLifetime - 1.0f)/2, rigidBodyMaxLifetime - 1.0f));
        yield return WaitAllRigidBodySleptOrTimeOut(rigidBodyMaxLifetime);

        yield return new WaitForSeconds(1.0f);

        if (doFading)
        {
            yield return FadeChild(fadeTime);

            yield return HideChild();
        }
    }

    public void BeginDemolish(float explosionForce, float explosionRadius, float upwardsModifier = 0.0f, ForceMode mode = ForceMode.Force)
    {
        // make sure visible
        this.gameObject.SetActive(true);
        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        meshRenderer.enabled = true;

        Initialize(explosionForce, explosionRadius, upwardsModifier, mode);
    }

    IEnumerator WaitAllRigidBodySleptOrTimeOut(float timeout)
    {
        float startTime = Time.time;

        bool done = false;
        while (!done/* && (Time.time-startTime<timeout)*/)
        {
            UpdatePhysics(true);

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