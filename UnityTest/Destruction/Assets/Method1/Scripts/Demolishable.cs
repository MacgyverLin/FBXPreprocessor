using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/////////////////////////////////////////////////////////
public class Fragment
{
    private GameObject fragmentObject;
    private Rigidbody rigidbody;
    private CapsuleCollider capsuleCollider;

    private FaceGroup faceGroup;
    private Matrix4x4 matrix4x4;

    public Fragment()
    {
        fragmentObject = GameObject.Instantiate(Resources.Load("CapsuleFragment", typeof(GameObject))) as GameObject;
        rigidbody = fragmentObject.GetComponent<Rigidbody>();
        capsuleCollider = fragmentObject.GetComponent<CapsuleCollider>();
    }

    public void Init(float mass = 1.0f, float drag = 0.0f, float angularDrag = 0.05f, bool useGravity = true, bool isKinematic = false, RigidbodyInterpolation interpolation = RigidbodyInterpolation.None, CollisionDetectionMode collisionDetectionMode = CollisionDetectionMode.Discrete)
    {
        rigidbody.mass = mass;
        rigidbody.drag = drag;
        rigidbody.angularDrag = angularDrag;
        rigidbody.useGravity = useGravity;
        rigidbody.isKinematic = isKinematic;
        rigidbody.interpolation = interpolation;
        rigidbody.collisionDetectionMode = collisionDetectionMode;
    }

    public void Explode(GameObject gameObject, FaceGroup faceGroup, float explosionForce, Vector3 explosionPosition, float explosionRadius, float upwardsModifier = 0.0f, ForceMode mode = ForceMode.Force)
    {
        this.faceGroup = faceGroup;
        fragmentObject.SetActive(true);

        fragmentObject.transform.position = gameObject.transform.TransformPoint(faceGroup.bound.center);
        fragmentObject.transform.rotation = gameObject.transform.rotation;
        fragmentObject.transform.localScale = gameObject.transform.localScale;

        rigidbody.position = fragmentObject.transform.position;
        rigidbody.rotation = fragmentObject.transform.rotation;

        //capsuleCollider.center = faceGroup.bound.center;
        capsuleCollider.radius = Mathf.Sqrt((faceGroup.bound.size.x * faceGroup.bound.size.x) + (faceGroup.bound.size.y * faceGroup.bound.size.y) + (faceGroup.bound.size.z * faceGroup.bound.size.z));
        capsuleCollider.height = capsuleCollider.radius * 2;

        //rigidbody.AddExplosionForce(explosionForce, explosionPosition, explosionRadius, upwardsModifier, mode);
        //rigidbody.AddExplosionForce(1.0f, gameObject.transform.position, 1.0f, 4.0f, mode);

        Update();
    }

    public void SetActive(bool active)
    {
        fragmentObject.SetActive(active);
    }

    public bool IsSleeping()
    {
        return rigidbody.IsSleeping();
    }

    public void Update()
    {
        Vector3 pivot = faceGroup.bound.center;

        Matrix4x4 m1 = Matrix4x4.identity;
        m1.SetTRS(-pivot, Quaternion.identity, Vector3.one);

        Matrix4x4 m2 = Matrix4x4.identity;
        m2.SetTRS(Vector3.zero, fragmentObject.transform.rotation, Vector3.one);

        Matrix4x4 m3 = Matrix4x4.identity;
        m3.SetTRS(Vector3.zero, Quaternion.identity, fragmentObject.transform.localScale);

        Matrix4x4 m4 = Matrix4x4.identity;
        m4.SetTRS(fragmentObject.transform.position, Quaternion.identity, Vector3.one);

        matrix4x4 = m4 * m3 * m2 * m1;
    }

    public Matrix4x4 GetTransform()
    {
        return matrix4x4;
    }
}

public class FragmentManager
{
    ///////////////////////////////////////////////////////////////
    private List<Fragment> activeFragments;
    private List<Fragment> inactiveFragments;
    private const int maxFragmentCount = 256;

    private static FragmentManager _instance;
    private FragmentManager()
    {
        activeFragments = new List<Fragment>();
        inactiveFragments = new List<Fragment>();

        for (int i = 0; i < maxFragmentCount; i++)
        {
            Fragment fragment = new Fragment();
            fragment.SetActive(false);
            inactiveFragments.Add(fragment);
        }
    }

    public static FragmentManager Instance()
    {
        if (_instance == null)
        {
            _instance = new FragmentManager();
        }
        return _instance;
    }

    public int GetMaxFragmentCount()
    {
        return maxFragmentCount;
    }

    public Fragment Alloc()
    {
        if (inactiveFragments.Count == 0)
            return null;

        Fragment fragment = inactiveFragments[0];
        fragment.SetActive(false);

        inactiveFragments.Remove(fragment);
        activeFragments.Add(fragment);

        return fragment;
    }

    public void Free(Fragment fragment)
    {
        Debug.Assert(activeFragments.Contains(fragment), "fragment is not in activelist");

        fragment.SetActive(false);

        activeFragments.Remove(fragment);
        inactiveFragments.Add(fragment);
    }
}


/////////////////////////////////////////////////////////
public class Demolishable : MonoBehaviour
{
    public DemolishableData demolishableData;
    private Fragment[] fragments;
    Matrix4x4[] transforms = new Matrix4x4[32];

    void Start()
    {
    }

    void Reset()
    {
    }

    public void Demolish(bool doFading, float rigidBodyMaxLifetime, float fadeTime, float explosionForce, Vector3 explosionPosition, float explosionRadius, float upwardsModifier = 1.0f, ForceMode mode = ForceMode.Force)
    {
        StartCoroutine(DemolishCoroutine(doFading, rigidBodyMaxLifetime, fadeTime, explosionForce, explosionPosition, explosionRadius, upwardsModifier, mode));
    }

    private IEnumerator DemolishCoroutine(bool doFading, float rigidBodyMaxLifetime, float fadeTime, float explosionForce, Vector3 explosionPosition, float explosionRadius, float upwardsModifier = 1.0f, ForceMode mode = ForceMode.Force)
    {
        BeginExplode(explosionForce, explosionPosition, explosionRadius, upwardsModifier, mode);
        
        yield return WaitAllRigidBodySleptOrTimeOut(rigidBodyMaxLifetime);

        yield return new WaitForSeconds(1.0f);

        yield return HideChild();

        EndExplode();

        yield return null;
    }

    public void BeginExplode(float explosionForce, Vector3 explosionPosition, float explosionRadius, float upwardsModifier = 1.0f, ForceMode mode = ForceMode.Force)
    {
        fragments = new Fragment[demolishableData.GetFaceGroupCount()];
        for (int i = 0; i < demolishableData.GetFaceGroupCount(); i++)
        {
            FaceGroup faceGroup = demolishableData.GetFaceGroup(i);

            fragments[i] = FragmentManager.Instance().Alloc();

            fragments[i].Init();
            fragments[i].Explode(this.gameObject, faceGroup, explosionForce, explosionPosition, explosionRadius, upwardsModifier, mode);
        }

        UpdateFragment(false);
    }

    IEnumerator WaitAllRigidBodySleptOrTimeOut(float timeout)
    {
        float startTime = Time.time;

        bool done = false;
        while (!done && (Time.time-startTime<timeout))
        {
            UpdateFragment(true);

            done = true;
            for(int i=0; i < fragments.Length; i++)
            {
                if (!fragments[i].IsSleeping())
                {
                    done = false;
                    break;
                }
            }

            yield return null;
        }
    }

    void UpdateFragment(bool showCrossSection)
    {
        for (int i = 0; i < fragments.Length; i++)
        {
            int groupID = demolishableData.GetFaceGroup(i).groupID;

            fragments[i].Update();
            transforms[groupID] = fragments[i].GetTransform();
        }

        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        for (int i = 0; i < meshRenderer.materials.Length; i++)
        {
            meshRenderer.materials[i].SetFloat("_IsDestructed", showCrossSection ? 1.0f : 0.0f);
            meshRenderer.materials[i].SetMatrixArray("_Transforms", transforms);
        }
    }

    IEnumerator HideChild()
    {
        MeshRenderer meshRenderer = this.GetComponent<MeshRenderer>();
        meshRenderer.enabled = false;

        yield return null;
    }

    public void EndExplode()
    {
        for (int i = 0; i < demolishableData.GetFaceGroupCount(); i++)
        {
            FragmentManager.Instance().Free(fragments[i]);
        }
    }
}