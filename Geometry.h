#ifndef _Geometry_h_
#define _Geometry_h_

#include "../Common/Common.h"
#include <vector>
#include <map>
#include <assert.h>

#define NUM_COLORS 1
#define NUM_UVS 8
#define NUM_NORMALS 1
#define NUM_TANGENTS 1
#define NUM_BINORMALS 1

#define NUM_VERTICES_PER_POLYGON 3

///////////////////////////////////////////////////////////////////
static float UnitRandom()
{
	return ((float)rand()) / RAND_MAX;
}

static float RangeRandom(float min, float max)
{
	return UnitRandom() * (max - min) + min;
}

static const float ZeroTolerance = 1e-4f;

class Vector2
{
public:
	Vector2()
	{
	}

	Vector2(const FbxVector4& v)
	{
		this->m[0] = (float)v[0];
		this->m[1] = (float)v[1];
	}

	Vector2(const FbxVector2& v)
	{
		this->m[0] = (float)v[0];
		this->m[1] = (float)v[1];
	}

	Vector2(float fX, float fY)
	{
		this->m[0] = fX;
		this->m[1] = fY;
	}

	Vector2(const float* m)
	{
		this->m[0] = m[0];
		this->m[1] = m[1];
	}

	Vector2(const Vector2& v)
	{
		this->m[0] = v.m[0];
		this->m[1] = v.m[1];
	}

	void Set(float fX, float fY)
	{
		this->m[0] = fX;
		this->m[1] = fY;
	}

	// coordinate access
	operator const float* () const
	{
		return m;
	}

	operator float* ()
	{
		return m;
	}

	float operator[] (int i) const
	{
		return m[i];
	}

	float& operator[] (int i)
	{
		return m[i];
	}

	float X() const
	{
		return m[0];
	}

	float& X()
	{
		return m[0];
	}

	float Y() const
	{
		return m[1];
	}

	float& Y()
	{
		return m[1];
	}

	// assignment
	Vector2& operator= (const FbxVector2& v)
	{
		this->m[0] = (float)v[0];
		this->m[1] = (float)v[1];

		return *this;
	}

	Vector2& operator= (const FbxVector4& v)
	{
		this->m[0] = (float)v[0];
		this->m[1] = (float)v[1];

		return *this;
	}

	Vector2& operator= (const Vector2& v)
	{
		this->m[0] = v.m[0];
		this->m[1] = v.m[1];

		return *this;
	}

	int CompareArrays(const Vector2& v) const
	{
		return memcmp(m, v.m, 2 * sizeof(float));
	}

	// comparison
	bool operator== (const Vector2& v) const
	{
		return CompareArrays(v) == 0;
	}

	bool operator!= (const Vector2& v) const
	{
		return CompareArrays(v) != 0;
	}

	bool operator<  (const Vector2& v) const
	{
		return CompareArrays(v) < 0;
	}

	bool operator<= (const Vector2& v) const
	{
		return CompareArrays(v) <= 0;
	}

	bool operator>  (const Vector2& v) const
	{
		return CompareArrays(v) > 0;
	}

	bool operator>= (const Vector2& v) const
	{
		return CompareArrays(v) >= 0;
	}

	// arithmetic operations
	Vector2 operator+ (const Vector2& v) const
	{
		return Vector2
		(
			m[0] + v.m[0],
			m[1] + v.m[1]
		);
	}

	Vector2 operator- (const Vector2& v) const
	{
		return Vector2
		(
			m[0] - v.m[0],
			m[1] - v.m[1]
		);
	}

	Vector2 operator* (const Vector2& v) const
	{
		return Vector2
		(
			m[0] * v.m[0],
			m[1] * v.m[1]
		);
	}

	Vector2 operator/ (const Vector2& v) const
	{
		return Vector2
		(
			m[0] / v.m[0],
			m[1] / v.m[1]
		);
	}

	Vector2 operator* (float fScalar) const
	{
		return Vector2
		(
			m[0] * fScalar,
			m[1] * fScalar
		);
	}

	Vector2 operator/ (float fScalar) const
	{
		return Vector2
		(
			m[0] / fScalar,
			m[1] / fScalar
		);
	}

	Vector2 operator- () const
	{
		return Vector2
		(
			-m[0],
			-m[1]
		);
	}

	// arithmetic updates
	Vector2& operator+= (const Vector2& v)
	{
		m[0] += v.m[0];
		m[1] += v.m[1];

		return *this;
	}

	Vector2& operator-= (const Vector2& v)
	{
		m[0] -= v.m[0];
		m[1] -= v.m[1];

		return *this;
	}

	Vector2& operator*= (const Vector2& v)
	{
		m[0] *= v.m[0];
		m[1] *= v.m[1];

		return *this;
	}

	Vector2& operator*= (float fScalar)
	{
		m[0] *= fScalar;
		m[1] *= fScalar;

		return *this;
	}

	Vector2& operator/= (float fScalar)
	{
		m[0] /= fScalar;
		m[1] /= fScalar;

		return *this;
	}

	// vector operations
	static Vector2 Min(const Vector2& v0, const Vector2& v1)
	{
		return Vector2(std::min(v0[0], v1[0]), std::min(v0[1], v1[1]));
	}

	static Vector2 Max(const Vector2& v0, const Vector2& v1)
	{
		return Vector2(std::max(v0[0], v1[0]), std::max(v0[1], v1[1]));
	}

	float Length() const
	{
		return (float)sqrt(SquaredLength());
	}

	float SquaredLength() const
	{
		return m[0] * m[0] + m[1] * m[1];
	}

	float Dot(const Vector2& v) const
	{
		return m[0] * v.m[0] + m[1] * v.m[1];
	}

	float Normalize()
	{
		float fLength = Length();

		if (fLength > 0.000001)
		{
			float fInvLength = ((float)1.0) / fLength;
			m[0] *= fInvLength;
			m[1] *= fInvLength;
		}
		else
		{
			fLength = (float)0.0;
			m[0] = (float)0.0;
			m[1] = (float)0.0;
		}

		return fLength;
	}

	Vector2 Perp() const
	{
		return Vector2(m[1], -m[0]);
	}

	Vector2 UnitPerp() const
	{
		Vector2 result = Perp();
		result.Normalize();

		return result;
	}

	friend Vector2 Lerp(const Vector2& v0, const Vector2& v1, float t)
	{
		return v0 * (1 - t) + v1 * t;
	}
private:
	float m[2];
};

///////////////////////////////////////////////////////////////////
class Vector3
{
public:
	Vector3()
	{
	}

	Vector3(const FbxVector4& v)
	{
		this->m[0] = (float)v[0];
		this->m[1] = (float)v[1];
		this->m[2] = (float)v[2];
	}

	Vector3(float fX, float fY, float fZ)
	{
		this->m[0] = fX;
		this->m[1] = fY;
		this->m[2] = fZ;
	}

	Vector3(const float* m)
	{
		this->m[0] = m[0];
		this->m[1] = m[1];
		this->m[2] = m[2];
	}

	Vector3(const Vector3& v)
	{
		this->m[0] = v.m[0];
		this->m[1] = v.m[1];
		this->m[2] = v.m[2];
	}

	void Set(float fX, float fY, float fZ)
	{
		this->m[0] = fX;
		this->m[1] = fY;
		this->m[2] = fZ;
	}

	// coordinate access
	operator const float* () const
	{
		return m;
	}

	operator float* ()
	{
		return m;
	}

	float operator[] (int i) const
	{
		return m[i];
	}

	float& operator[] (int i)
	{
		return m[i];
	}

	float X() const
	{
		return m[0];
	}

	float& X()
	{
		return m[0];
	}

	float Y() const
	{
		return m[1];
	}

	float& Y()
	{
		return m[1];
	}

	float Z() const
	{
		return m[2];
	}

	float& Z()
	{
		return m[2];
	}

	// assignment
	Vector3& operator= (const FbxVector4& v)
	{
		this->m[0] = (float)v[0];
		this->m[1] = (float)v[1];
		this->m[2] = (float)v[2];

		return *this;
	}

	Vector3& operator= (const Vector3& v)
	{
		this->m[0] = v.m[0];
		this->m[1] = v.m[1];
		this->m[2] = v.m[2];

		return *this;
	}

	int CompareArrays(const Vector3& v) const
	{
		return memcmp(m, v.m, 3 * sizeof(float));
	}

	// comparison
	friend Vector3 Min(const Vector3& v0, const Vector3& v1)
	{
		return Vector3(std::min(v0[0], v1[0]), std::min(v0[1], v1[1]), std::min(v0[2], v1[2]));
	}

	friend Vector3 Max(const Vector3& v0, const Vector3& v1)
	{
		return Vector3(std::max(v0[0], v1[0]), std::max(v0[1], v1[1]), std::max(v0[2], v1[2]));
	}

	bool operator== (const Vector3& v) const
	{
		return CompareArrays(v) == 0;
	}

	bool operator!= (const Vector3& v) const
	{
		return CompareArrays(v) != 0;
	}

	bool operator<  (const Vector3& v) const
	{
		return CompareArrays(v) < 0;
	}

	bool operator<= (const Vector3& v) const
	{
		return CompareArrays(v) <= 0;
	}

	bool operator>  (const Vector3& v) const
	{
		return CompareArrays(v) > 0;
	}

	bool operator>= (const Vector3& v) const
	{
		return CompareArrays(v) >= 0;
	}

	// arithmetic operations
	Vector3 operator+ (const Vector3& v) const
	{
		return Vector3
		(
			m[0] + v.m[0],
			m[1] + v.m[1],
			m[2] + v.m[2]
		);
	}

	Vector3 operator- (const Vector3& v) const
	{
		return Vector3
		(
			m[0] - v.m[0],
			m[1] - v.m[1],
			m[2] - v.m[2]
		);
	}

	Vector3 operator* (const Vector3& v) const
	{
		return Vector3
		(
			m[0] * v.m[0],
			m[1] * v.m[1],
			m[2] * v.m[2]
		);
	}

	Vector3 operator/ (const Vector3& v) const
	{
		return Vector3
		(
			m[0] / v.m[0],
			m[1] / v.m[1],
			m[2] / v.m[2]
		);
	}

	Vector3 operator* (float fScalar) const
	{
		return Vector3
		(
			m[0] * fScalar,
			m[1] * fScalar,
			m[2] * fScalar
		);
	}

	Vector3 operator/ (float fScalar) const
	{
		return Vector3
		(
			m[0] / fScalar,
			m[1] / fScalar,
			m[2] / fScalar
		);
	}

	Vector3 operator- () const
	{
		return Vector3
		(
			-m[0],
			-m[1],
			-m[2]
		);
	}

	// arithmetic updates
	Vector3& operator+= (const Vector3& v)
	{
		m[0] += v.m[0];
		m[1] += v.m[1];
		m[2] += v.m[2];
		
		return *this;
	}

	Vector3& operator-= (const Vector3& v)
	{
		m[0] -= v.m[0];
		m[1] -= v.m[1];
		m[2] -= v.m[2];

		return *this;
	}

	Vector3& operator*= (const Vector3& v)
	{
		m[0] *= v.m[0];
		m[1] *= v.m[1];
		m[2] *= v.m[2];

		return *this;
	}

	Vector3& operator*= (float fScalar)
	{
		m[0] *= fScalar;
		m[1] *= fScalar;
		m[2] *= fScalar;

		return *this;
	}

	Vector3& operator/= (float fScalar)
	{
		m[0] /= fScalar;
		m[1] /= fScalar;
		m[2] /= fScalar;

		return *this;
	}

	// vector operations
	float Length() const
	{
		return (float)sqrt(SquaredLength());
	}

	float SquaredLength() const
	{
		return m[0] * m[0] + m[1] * m[1] + m[2] * m[2];
	}

	float Dot(const Vector3& v) const
	{
		return m[0] * v.m[0] + m[1] * v.m[1] + m[2] * v.m[2];
	}

	float Normalize()
	{
		float fLength = Length();

		if (fLength > 0.000001)
		{
			float fInvLength = ((float)1.0) / fLength;
			m[0] *= fInvLength;
			m[1] *= fInvLength;
			m[2] *= fInvLength;
		}
		else
		{
			fLength = (float)0.0;
			m[0] = (float)0.0;
			m[1] = (float)0.0;
			m[2] = (float)0.0;
		}

		return fLength;
	}

	Vector3 Cross(const Vector3& v) const
	{
		return Vector3(
			m[1] * v.m[2] - m[2] * v.m[1],
			m[2] * v.m[0] - m[0] * v.m[2],
			m[0] * v.m[1] - m[1] * v.m[0]);
	}

	Vector3 UnitCross(const Vector3& v) const
	{
		Vector3 result = Cross(v);
		result.Normalize();

		return result;
	}

	friend Vector3 Lerp(const Vector3& v0, const Vector3& v1, float t)
	{
		return v0 * (1 - t) + v1 * t;
	}
private:
	float m[3];
};

class AABB
{
public:
	AABB()
	{
	}

	AABB(const Vector3& min, const Vector3& max)
	{
		this->min = Min(min, max);
		this->max = Max(min, max);
	}

	Vector3 GetExtent() const
	{
		return max - min;
	}

	friend AABB operator + (const AABB& a0, const AABB& a1)
	{
		AABB result;
		result.min = Min(a0.min, a1.min);
		result.max = Max(a0.max, a1.max);

		return result;
	}

	AABB operator += (const AABB& aabb)
	{
		this->min = Min(min, aabb.min);
		this->max = Max(max, aabb.max);

		return *this;
	}

	Vector3 GetMajorAxis(bool randomize = false) const
	{
		Vector3 extent(GetExtent());

		float maxv = randomize ? 0.3f : 0.0f;

		Vector3 result;
		if (extent.X() > extent.Y() && extent.X() > extent.Z()) // X
			result = Vector3(1.0, RangeRandom(0, maxv), RangeRandom(0, maxv));
		else if (extent.Y() > extent.X() && extent.Y() > extent.Z()) // Y
			result = Vector3(RangeRandom(0, maxv), 1.0, RangeRandom(0, maxv));
		else // Z
			result = Vector3(RangeRandom(0, maxv), RangeRandom(0, maxv), 1.0);

		result.Normalize();

		return result;
	}

	Vector3 GetCenter(bool randomize = false) const
	{
		float minv;
		float maxv;
		if (randomize)
		{
			minv = 0.3f;
			maxv = 0.7f;
		}
		else
		{
			minv = 0.5f;
			maxv = 0.5f;
		}

		return min + GetExtent() * Vector3(RangeRandom(minv, maxv), RangeRandom(minv, maxv), RangeRandom(minv, maxv));
	}

	const Vector3& GetMin() const
	{
		return min;
	}

	const Vector3& GetMax() const
	{
		return max;
	}
private:
	Vector3 min;
	Vector3 max;
};

///////////////////////////////////////////////////////////////////
class Color
{
public:
	Color()
	{
	}

	Color(const FbxVector4& v)
	{
		this->m[0] = (float)v[0];
		this->m[1] = (float)v[1];
		this->m[2] = (float)v[2];
		this->m[3] = (float)v[3];
	}

	Color(float r, float g, float b, float a)
	{
		this->m[0] = r;
		this->m[1] = g;
		this->m[2] = b;
		this->m[3] = a;
	}

	Color(const float* m)
	{
		this->m[0] = m[0];
		this->m[1] = m[1];
		this->m[2] = m[2];
		this->m[3] = m[3];
	}

	Color(const Color& v)
	{
		this->m[0] = v.m[0];
		this->m[1] = v.m[1];
		this->m[2] = v.m[2];
		this->m[3] = v.m[3];
	}

	void Set(float r, float g, float b, float a)
	{
		this->m[0] = r;
		this->m[1] = g;
		this->m[2] = b;
		this->m[3] = a;
	}

	// coordinate access
	operator const float* () const
	{
		return m;
	}

	operator float* ()
	{
		return m;
	}

	float operator[] (int i) const
	{
		return m[i];
	}

	float& operator[] (int i)
	{
		return m[i];
	}

	float R() const
	{
		return m[0];
	}

	float& R()
	{
		return m[0];
	}

	float G() const
	{
		return m[1];
	}

	float& G()
	{
		return m[1];
	}

	float B() const
	{
		return m[2];
	}

	float& B()
	{
		return m[2];
	}

	float A() const
	{
		return m[3];
	}

	float& A()
	{
		return m[3];
	}

	static Color Min(const Color& v0, const Color& v1)
	{
		return Color(std::min(v0[0], v1[0]), std::min(v0[1], v1[1]), std::min(v0[2], v1[2]), std::min(v0[3], v1[3]));
	}

	static Color Max(const Color& v0, const Color& v1)
	{
		return Color(std::max(v0[0], v1[0]), std::max(v0[1], v1[1]), std::max(v0[2], v1[2]), std::max(v0[3], v1[3]));
	}

	// assignment
	Color& operator= (const FbxColor& v)
	{
		this->m[0] = (float)v[0];
		this->m[1] = (float)v[1];
		this->m[2] = (float)v[2];
		this->m[3] = (float)v[3];

		return *this;
	}

	Color& operator= (const Color& v)
	{
		this->m[0] = v.m[0];
		this->m[1] = v.m[1];
		this->m[2] = v.m[2];
		this->m[3] = v.m[3];

		return *this;
	}

	int CompareArrays(const Color& v) const
	{
		return memcmp(m, v.m, 4 * sizeof(float));
	}

	// comparison
	bool operator== (const Color& v) const
	{
		return CompareArrays(v) == 0;
	}

	bool operator!= (const Color& v) const
	{
		return CompareArrays(v) != 0;
	}

	bool operator<  (const Color& v) const
	{
		return CompareArrays(v) < 0;
	}

	bool operator<= (const Color& v) const
	{
		return CompareArrays(v) <= 0;
	}

	bool operator>  (const Color& v) const
	{
		return CompareArrays(v) > 0;
	}

	bool operator>= (const Color& v) const
	{
		return CompareArrays(v) >= 0;
	}

	// arithmetic operations
	Color operator+ (const Color& v) const
	{
		return Color
		(
			m[0] + v.m[0],
			m[1] + v.m[1],
			m[2] + v.m[2],
			m[3] + v.m[3]
		);
	}

	Color operator- (const Color& v) const
	{
		return Color
		(
			m[0] - v.m[0],
			m[1] - v.m[1],
			m[2] - v.m[2],
			m[3] - v.m[3]
		);
	}

	Color operator* (const Color& v) const
	{
		return Color
		(
			m[0] * v.m[0],
			m[1] * v.m[1],
			m[2] * v.m[2],
			m[3] * v.m[3]
		);
	}

	Color operator/ (const Color& v) const
	{
		return Color
		(
			m[0] / v.m[0],
			m[1] / v.m[1],
			m[2] / v.m[2],
			m[3] / v.m[3]
		);
	}

	Color operator* (float fScalar) const
	{
		return Color
		(
			m[0] * fScalar,
			m[1] * fScalar,
			m[2] * fScalar,
			m[3] * fScalar
		);
	}

	Color operator/ (float fScalar) const
	{
		return Color
		(
			m[0] / fScalar,
			m[1] / fScalar,
			m[2] / fScalar,
			m[3] / fScalar
		);
	}

	Color operator- () const
	{
		return Color
		(
			-m[0],
			-m[1],
			-m[2],
			-m[3]
		);
	}

	// arithmetic updates
	Color& operator+= (const Color& v)
	{
		m[0] += v.m[0];
		m[1] += v.m[1];
		m[2] += v.m[2];
		m[3] += v.m[3];

		return *this;
	}

	Color& operator-= (const Color& v)
	{
		m[0] -= v.m[0];
		m[1] -= v.m[1];
		m[2] -= v.m[2];
		m[3] -= v.m[3];

		return *this;
	}

	Color& operator*= (const Color& v)
	{
		m[0] *= v.m[0];
		m[1] *= v.m[1];
		m[2] *= v.m[2];
		m[3] *= v.m[3];

		return *this;
	}

	Color& operator*= (float fScalar)
	{
		m[0] *= fScalar;
		m[1] *= fScalar;
		m[2] *= fScalar;
		m[3] *= fScalar;

		return *this;
	}

	Color& operator/= (float fScalar)
	{
		m[0] /= fScalar;
		m[1] /= fScalar;
		m[2] /= fScalar;
		m[3] /= fScalar;

		return *this;
	}

	// vector operations
	float Length() const
	{
		return (float)sqrt(SquaredLength());
	}

	float SquaredLength() const
	{
		return m[0] * m[0] + m[1] * m[1] + m[2] * m[2] + m[3] * m[3];
	}

	float Dot(const Color& v) const
	{
		return m[0] * v.m[0] + m[1] * v.m[1] + m[2] * v.m[2] + m[3] * v.m[3];
	}

	float Normalize()
	{
		float fLength = Length();

		if (fLength > 0.000001)
		{
			float fInvLength = ((float)1.0) / fLength;
			m[0] *= fInvLength;
			m[1] *= fInvLength;
			m[2] *= fInvLength;
			m[3] *= fInvLength;
		}
		else
		{
			fLength = (float)0.0;
			m[0] = (float)0.0;
			m[1] = (float)0.0;
			m[2] = (float)0.0;
			m[3] = (float)0.0;
		}

		return fLength;
	}

	friend Color Lerp(const Color& v0, const Color& v1, float t)
	{
		return v0 * (1 - t) + v1 * t;
	}
private:
	float m[4];
};

class Matrix4
{
public:
	Matrix4()
	{
	}

	~Matrix4()
	{
	}

	Matrix4(const Vector3& axis, float angle)
	{
		InitRotateAxisAngle(axis, angle);
	}

	Matrix4(const Vector3& col0, const Vector3& col1, const Vector3& col2, const Vector3& col3)
	{
		Init(col0, col1, col2, col3);
	}

	void InitZero()
	{
		m[0] = 0.0;
		m[1] = 0.0;
		m[2] = 0.0;
		m[3] = 0.0;

		m[4] = 0.0;
		m[5] = 0.0;
		m[6] = 0.0;
		m[7] = 0.0;

		m[8] = 0.0;
		m[9] = 0.0;
		m[10] = 0.0;
		m[11] = 0.0;

		m[12] = 0.0;
		m[13] = 0.0;
		m[14] = 0.0;
		m[15] = 0.0;
	}

	void InitIdentity()
	{
		m[0] = 1.0;
		m[1] = 0.0;
		m[2] = 0.0;
		m[3] = 0.0;

		m[4] = 0.0;
		m[5] = 1.0;
		m[6] = 0.0;
		m[7] = 0.0;

		m[8] = 0.0;
		m[9] = 0.0;
		m[10] = 1.0;
		m[11] = 0.0;

		m[12] = 0.0;
		m[13] = 0.0;
		m[14] = 0.0;
		m[15] = 1.0;
	}

	void Init(const Vector3& col0, const Vector3& col1, const Vector3& col2, const Vector3& col3)
	{
		m[0] = col0[0];
		m[1] = col1[0];
		m[2] = col2[0];
		m[3] = col3[0];

		m[4] = col0[1];
		m[5] = col1[1];
		m[6] = col2[1];
		m[7] = col3[1];

		m[8] = col0[2];
		m[9] = col1[2];
		m[10] = col2[2];
		m[11] = col3[2];

		m[12] = 0.0;
		m[13] = 0.0;
		m[14] = 0.0;
		m[15] = 1.0;
	}

	void InitRotateAxisAngle(const Vector3& axis, float angle)
	{
		float fCos = (float)cos(-angle / 180.0f * 3.141592654f);
		float fSin = (float)sin(-angle / 180.0f * 3.141592654f);
		float fOneMinusCos = 1.0f - fCos;
		float fX2 = axis[0] * axis[0];
		float fY2 = axis[1] * axis[1];
		float fZ2 = axis[2] * axis[2];
		float fXYM = axis[0] * axis[1] * fOneMinusCos;
		float fXZM = axis[0] * axis[2] * fOneMinusCos;
		float fYZM = axis[1] * axis[2] * fOneMinusCos;
		float fXSin = axis[0] * fSin;
		float fYSin = axis[1] * fSin;
		float fZSin = axis[2] * fSin;

		m[0] = fX2 * fOneMinusCos + fCos;
		m[1] = fXYM + fZSin;
		m[2] = fXZM - fYSin;
		m[3] = 0.0;

		m[4] = fXYM - fZSin;
		m[5] = fY2 * fOneMinusCos + fCos;
		m[6] = fYZM + fXSin;
		m[7] = 0.0;

		m[8] = fXZM + fYSin;
		m[9] = fYZM - fXSin;
		m[10] = fZ2 * fOneMinusCos + fCos;
		m[11] = 0.0;

		m[12] = 0.0;
		m[13] = 0.0;
		m[14] = 0.0;
		m[15] = 1.0;
	}

	operator const float* () const
	{
		return m;
	}

	operator float* ()
	{
		return m;
	}

	const float* operator[] (int iRow) const
	{
		return &m[4 * iRow];
	}

	float* operator[] (int iRow)
	{
		return &m[4 * iRow];
	}

	float operator() (int iRow, int iCol) const
	{
		return m[I(iRow, iCol)];
	}

	float& operator() (int iRow, int iCol)
	{
		return m[I(iRow, iCol)];
	}

	int I(int iRow, int iCol) const
	{
		assert(0 <= iRow && iRow < 4 && 0 <= iCol && iCol < 4);

		return iCol + 4 * iRow;
	}

	Matrix4 Inverse() const
	{
		float fA0 = m[0] * m[5] - m[1] * m[4];
		float fA1 = m[0] * m[6] - m[2] * m[4];
		float fA2 = m[0] * m[7] - m[3] * m[4];
		float fA3 = m[1] * m[6] - m[2] * m[5];
		float fA4 = m[1] * m[7] - m[3] * m[5];
		float fA5 = m[2] * m[7] - m[3] * m[6];
		float fB0 = m[8] * m[13] - m[9] * m[12];
		float fB1 = m[8] * m[14] - m[10] * m[12];
		float fB2 = m[8] * m[15] - m[11] * m[12];
		float fB3 = m[9] * m[14] - m[10] * m[13];
		float fB4 = m[9] * m[15] - m[11] * m[13];
		float fB5 = m[10] * m[15] - m[11] * m[14];

		float fDet = fA0 * fB5 - fA1 * fB4 + fA2 * fB3 + fA3 * fB2 - fA4 * fB1 + fA5 * fB0;
		if (abs(fDet) <= 0.000001)
		{
			Matrix4 matZero;
			matZero.InitZero();

			return matZero;
		}

		Matrix4 kInv;
		kInv[0][0] = +m[5] * fB5 - m[6] * fB4 + m[7] * fB3;
		kInv[1][0] = -m[4] * fB5 + m[6] * fB2 - m[7] * fB1;
		kInv[2][0] = +m[4] * fB4 - m[5] * fB2 + m[7] * fB0;
		kInv[3][0] = -m[4] * fB3 + m[5] * fB1 - m[6] * fB0;
		kInv[0][1] = -m[1] * fB5 + m[2] * fB4 - m[3] * fB3;
		kInv[1][1] = +m[0] * fB5 - m[2] * fB2 + m[3] * fB1;
		kInv[2][1] = -m[0] * fB4 + m[1] * fB2 - m[3] * fB0;
		kInv[3][1] = +m[0] * fB3 - m[1] * fB1 + m[2] * fB0;
		kInv[0][2] = +m[13] * fA5 - m[14] * fA4 + m[15] * fA3;
		kInv[1][2] = -m[12] * fA5 + m[14] * fA2 - m[15] * fA1;
		kInv[2][2] = +m[12] * fA4 - m[13] * fA2 + m[15] * fA0;
		kInv[3][2] = -m[12] * fA3 + m[13] * fA1 - m[14] * fA0;
		kInv[0][3] = -m[9] * fA5 + m[10] * fA4 - m[11] * fA3;
		kInv[1][3] = +m[8] * fA5 - m[10] * fA2 + m[11] * fA1;
		kInv[2][3] = -m[8] * fA4 + m[9] * fA2 - m[11] * fA0;
		kInv[3][3] = +m[8] * fA3 - m[9] * fA1 + m[10] * fA0;

		float fInvDet = ((float)1.0) / fDet;
		for (int iRow = 0; iRow < 4; iRow++)
		{
			for (int iCol = 0; iCol < 4; iCol++)
			{
				kInv[iRow][iCol] *= fInvDet;
			}
		}

		return kInv;
	}

	Vector3 TimesPositionVector(const Vector3& v) const
	{
		Vector3 result;
		result[0] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
		result[1] = m[4] * v[0] + m[5] * v[1] + m[6] * v[2] + m[7];
		result[2] = m[8] * v[0] + m[9] * v[1] + m[10] * v[2] + m[11];

		return result;
	}

	Vector3 TimesDirectionVector(const Vector3& v) const
	{
		Vector3 result;
		result[0] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2];
		result[1] = m[4] * v[0] + m[5] * v[1] + m[6] * v[2];
		result[2] = m[8] * v[0] + m[9] * v[1] + m[10] * v[2];

		return result;
	}
private:
private:
	float m[16];
};

class Plane
{
public:
	Plane()
	{
	}

	Plane(const Plane& other)
	{
		this->n = other.n;
		this->d = other.d;
	}

	Plane(const Vector3& n, float d)
	{
		this->n = n;
		this->d = d;
	}

	Plane(const Vector3& n, const Vector3& p)
	{
		this->n = n;
		this->d = n.Dot(p);
	}

	Plane(const Vector3& p0, const Vector3& p1, const Vector3& p2)
	{
		Vector3 e0 = p1 - p0;
		Vector3 e1 = p2 - p0;

		this->n = e0.UnitCross(e1);
		this->d = this->n.Dot(p0);
	}

	Plane& operator= (const Plane& other)
	{
		this->n = other.n;
		this->d = other.d;

		return *this;
	}

	void Flip()
	{
		n = -n;
		d = -d;
	}

	bool WhichSide(const Vector3& p) const
	{
		float dist = DistanceTo(p);

		if (dist >= (float)0.0)
			return true;
		else
			return false;			
	}

	float DistanceTo(const Vector3& p) const
	{
		return n.Dot(p) - d;
	}

	bool isCoplanar(const Plane& other) const
	{
		return	fabs(n[0] - other.n[0]) < 0.00001f &&
			fabs(n[1] - other.n[1]) < 0.00001f &&
			fabs(n[2] - other.n[2]) < 0.00001f &&
			fabs(d - other.d) < 0.00001f;
	}

	const Vector3& Normal() const
	{
		return n;
	}

	float D() const
	{
		return d;
	}
private:
	Vector3 n;
	float d;
};


///////////////////////////////////////////////////////////////////
class Vertex
{
	friend class MeshBuilder;
	friend class MeshSlicer;
	friend class BSPMeshSlicer;
	friend class FBXMeshBuilder;
	friend class Polygon;
	friend class BSP;
public:
	Vertex()
	{
	}

	Vertex(const Vector3& position, const Vector3& normal, const Vector2& uv)
	{
		this->position = position;

		for (size_t i = 0; i < NUM_COLORS; i++)
			this->colors[i] = Color(1.0, 1.0, 1.0, 1.0);

		for (size_t i = 0; i < NUM_UVS; i++)
			this->uvs[i] = uv;

		for (size_t i = 0; i < NUM_NORMALS; i++)
			this->normals[i] = normal;

		for (size_t i = 0; i < NUM_TANGENTS; i++)
			this->tangents[i] = normal;

		for (size_t i = 0; i < NUM_BINORMALS; i++)
			this->binormals[i] = normal;
	}

	Vertex(const Vertex& other)
	{
		memcpy(this, &other, sizeof(Vertex));
	}

	Vertex& operator = (const Vertex& other)
	{
		memcpy(this, &other, sizeof(Vertex));

		return *this;
	}

	friend Vertex Lerp(const Vertex& v0, const Vertex& v1, float t)
	{
		Vertex result;

		result.position = Lerp(v0.position, v1.position, t);

		for (size_t i = 0; i < NUM_COLORS; i++)
			result.colors[i] = Lerp(v0.colors[i], v1.colors[i], t);

		for (size_t i = 0; i < NUM_UVS; i++)
			result.uvs[i] = Lerp(v0.uvs[i], v1.uvs[i], t);

		for (size_t i = 0; i < NUM_NORMALS; i++)
			result.normals[i] = Lerp(v0.normals[i], v1.normals[i], t);

		for (size_t i = 0; i < NUM_TANGENTS; i++)
			result.tangents[i] = Lerp(v0.tangents[i], v1.tangents[i], t);

		for (size_t i = 0; i < NUM_BINORMALS; i++)
			result.binormals[i] = Lerp(v0.binormals[i], v1.binormals[i], t);

		return result;
	}
private:
	Vector3 position;
	Color colors[NUM_COLORS];
	Vector2 uvs[NUM_UVS];
	Vector3 normals[NUM_NORMALS];
	Vector3 tangents[NUM_TANGENTS];
	Vector3 binormals[NUM_BINORMALS];
};

class Polygon
{
	friend class MeshBuilder;
	friend class MeshSlicer;
	friend class BSPMeshSlicer;
	friend class FBXMeshBuilder;
	friend class BSP;
public:
	Polygon()
	{
		materialIdx = 0;
	}

	Polygon(int materialIdx, const std::vector<Vertex>& vertices)
	{
		this->materialIdx = materialIdx;
		this->vertices = vertices;
	}

	~Polygon()
	{
	}

	Polygon(const Polygon& other)
	{
		materialIdx = other.materialIdx;
		vertices = other.vertices;
	}

	Polygon& operator = (const Polygon& other)
	{
		materialIdx = other.materialIdx;
		vertices = other.vertices;

		return *this;
	}

	void Clear()
	{
		materialIdx = 0;
		vertices.clear();
	}

	void Flip()
	{
		std::vector<Vertex> oldVertices = vertices;
		for (size_t i = 0; i < vertices.size(); i++)
		{
			vertices[i] = oldVertices[oldVertices.size() - 1 - i];
		}
	}

	bool IsEmpty() const
	{
		return vertices.size() == 0;
	}

	void Transform(const Vector3& t, const Vector3& s)
	{
		for (size_t i = 0; i < vertices.size(); i++)
		{
			vertices[i].position = vertices[i].position * s + t;
		}
	}
private:
	int materialIdx;
	std::vector<Vertex> vertices;
};

class BSP
{
public:
	//////////////////////////////////////////////////////////////////////////////////////////////////
	enum PolygonType
	{
		Coplanar = 0,
		Front = 1,
		Back = 2,
		Spanning = 3,
	};

	BSP()
		: nodeSplitPlane(nullptr)
		, nodePolygons()
		, front(nullptr)
		, back(nullptr)
	{
	}

	BSP(const std::vector<Polygon>& polygons)
		: nodeSplitPlane(nullptr)
		, nodePolygons()
		, front(nullptr)
		, back(nullptr)
	{
		FromPolygons(polygons);
	}

	~BSP()
	{
		if (nodeSplitPlane)
		{
			delete nodeSplitPlane;
			nodeSplitPlane = nullptr;
		}

		if (front)
		{
			delete front;
			front = nullptr;
		}

		if (back)
		{
			delete back;
			back = nullptr;
		}
	}

	BSP(const BSP& bsp)
		: nodeSplitPlane(bsp.nodeSplitPlane)
		, nodePolygons(bsp.nodePolygons)
		, front(bsp.front)
		, back(bsp.back)
	{
	}

	BSP& operator = (const BSP& bsp)
	{
		this->nodeSplitPlane = bsp.nodeSplitPlane;
		this->nodePolygons = bsp.nodePolygons;
		this->front = bsp.front;
		this->back = bsp.back;

		return *this;
	}

	void Invert()
	{
		for (int i = 0; i < this->nodePolygons.size(); i++)
			this->nodePolygons[i].Flip();

		if (this->nodeSplitPlane)
			this->nodeSplitPlane->Flip();

		if (this->front)
			this->front->Invert();

		if (this->back)
			this->back->Invert();

		std::swap(this->front, this->back);
	}

	std::vector<Polygon> ClipPolygons(const std::vector<Polygon>& polygons) const
	{
		if (!this->nodeSplitPlane)
			return polygons;

		std::vector<Polygon> frontPolygons;
		std::vector<Polygon> backPolygons;
		for (int i = 0; i < polygons.size(); i++)
			SplitPolygon(*nodeSplitPlane, polygons[i], frontPolygons, backPolygons, frontPolygons, backPolygons);

		if (this->front)
			frontPolygons = this->front->ClipPolygons(frontPolygons);

		if (this->back)
			backPolygons = this->back->ClipPolygons(backPolygons);
		else
			backPolygons.clear();

		Concat(frontPolygons, backPolygons);
		return frontPolygons;
	}

	void ClipBy(const BSP& bsp)
	{
		this->nodePolygons = bsp.ClipPolygons(this->nodePolygons);

		if (this->front)
			this->front->ClipBy(bsp);
		if (this->back)
			this->back->ClipBy(bsp);
	}

	void ToPolygons(std::vector<Polygon>& polygons) const
	{
		Concat(polygons, this->nodePolygons);

		if (this->front)
			this->front->ToPolygons(polygons);

		if (this->back)
			this->back->ToPolygons(polygons);
	}

	Plane GetPlane(const Polygon& polygon) const
	{
		return Plane(polygon.vertices[0].position, polygon.vertices[1].position, polygon.vertices[2].position);
	}

	void FromPolygons(const std::vector<Polygon>& polygons)
	{
		if (polygons.size() == 0)
			return;

		int splitPlaneIdx = -1;
		if (!this->nodeSplitPlane)
		{
			splitPlaneIdx = GetSplitPlaneIdx(polygons);
			this->nodeSplitPlane = new Plane(GetPlane(polygons[splitPlaneIdx]));

			this->nodePolygons.push_back(polygons[splitPlaneIdx]);
		}

		std::vector<Polygon> frontPolygons;
		std::vector<Polygon> backPolygons;
		for (int i = 0; i < polygons.size(); i++)
		{
			if (i != splitPlaneIdx)
				SplitPolygon(*nodeSplitPlane, polygons[i], this->nodePolygons, this->nodePolygons, frontPolygons, backPolygons);
		}

		if (frontPolygons.size())
		{
			if (!this->front)
				this->front = new BSP();

			this->front->FromPolygons(frontPolygons);
		}

		if (backPolygons.size())
		{
			if (!this->back)
				this->back = new BSP();

			this->back->FromPolygons(backPolygons);
		}
	}
protected:
	void SplitPolygon(
		const Plane& splitPlane,
		const Polygon& polygon,
		std::vector<Polygon>& coplanarFrontPolygons,
		std::vector<Polygon>& coplanarBackPolygons,
		std::vector<Polygon>& frontPolygons,
		std::vector<Polygon>& backPolygons) const
	{
		int polygonType = 0;
		std::vector<int> types(polygon.vertices.size());
		for (int i = 0; i < polygon.vertices.size(); i++)
		{
			float d = splitPlane.DistanceTo(polygon.vertices[i].position);
			int type = 0;
			if (d < -ZeroTolerance)
				type = PolygonType::Back;
			else if (d > ZeroTolerance)
				type = PolygonType::Front;
			else
				type = PolygonType::Coplanar;

			polygonType |= type;
			types[i] = type;
		}

		std::vector<Vertex> frontVertices;
		std::vector<Vertex> backVertices;

		Plane p = GetPlane(polygon);
		switch (polygonType)
		{
		case PolygonType::Coplanar:
			(splitPlane.Normal().Dot(p.Normal()) > 0 ? coplanarFrontPolygons : coplanarBackPolygons).push_back(polygon);
			break;
		case PolygonType::Front:
			frontPolygons.push_back(polygon);
			break;
		case PolygonType::Back:
			backPolygons.push_back(polygon);
			break;
		case PolygonType::Spanning:
			for (int i = 0; i < polygon.vertices.size(); i++)
			{
				int j = (i + 1) % polygon.vertices.size();
				int ti = types[i];
				int tj = types[j];
				const Vertex& vi = polygon.vertices[i];
				const Vertex& vj = polygon.vertices[j];
				if (ti != PolygonType::Back)
					frontVertices.push_back(vi);
				if (ti != PolygonType::Front)
					backVertices.push_back(vi);
				if ((ti | tj) == PolygonType::Spanning)
				{
					const Vector3& normal = splitPlane.Normal();
					float t = (splitPlane.D() - normal.Dot(vi.position)) / normal.Dot(vj.position - vi.position);
					Vertex vc = Lerp(vi, vj, t);

					frontVertices.push_back(vc);
					backVertices.push_back(vc);
				}
			}

			if (frontVertices.size() >= 3)
				frontPolygons.push_back(Polygon(polygon.materialIdx, frontVertices));

			if (backVertices.size() >= 3)
				backPolygons.push_back(Polygon(polygon.materialIdx, backVertices));
			break;
		}
	}

	void Concat(std::vector<Polygon>& dst, const std::vector<Polygon>& src) const
	{
		dst.insert(dst.end(), src.begin(), src.end());
	}

	void GetPolygonsType(const Plane& splitPlane, const Polygon& polygon, int& coplanerCount, int& frontCount, int& backCount, int& spanCount) const
	{
		coplanerCount = 0;
		frontCount = 0;
		backCount = 0;
		spanCount = 0;

		int polygonType = 0;
		for (int i = 0; i < polygon.vertices.size(); i++)
		{
			float d = splitPlane.DistanceTo(polygon.vertices[i].position);
			int type = 0;
			if (d < -ZeroTolerance)
				type = PolygonType::Back;
			else if (d > ZeroTolerance)
				type = PolygonType::Front;
			else
				type = PolygonType::Coplanar;

			polygonType |= type;
		}

		switch (polygonType)
		{
		case PolygonType::Coplanar:
			coplanerCount++;
			break;

		case PolygonType::Front:
			frontCount++;
			break;

		case PolygonType::Back:
			backCount++;
			break;

		case PolygonType::Spanning:
			spanCount++;
			break;
		}
	}

	int GetSplitPlaneIdx(const std::vector<Polygon>& polygons) const
	{
		return 0;
		/*
		int coplanerCount = 0;
		int frontCount = 0;
		int backCount = 0;
		int spanCount = 0;

		int minSpan = 0x7fffffff;
		int minSpanIdx = 0;
		int minDiff = 0x7fffffff;
		int minDiffIdx = 0;
		for (int i = 0; i < polygons.size(); i++)
		{
			for (int j = 0; j < polygons.size(); j++)
			{
				GetPolygonsType(polygons[i].GetPlane(), polygons[j], coplanerCount, frontCount, backCount, spanCount);
				if (minSpan > spanCount)
				{
					minSpanIdx = i;
					minSpan = spanCount;
				}

				int diff = abs(frontCount - backCount);
				if (minDiff > diff)
				{
					minDiffIdx = i;
					minDiff = diff;
				}
			}
		}

		return minSpanIdx;
		return minDiffIdx;
		*/
	}
private:

public:
protected:
private:
	Plane* nodeSplitPlane;
	std::vector<Polygon> nodePolygons;

	BSP* front;
	BSP* back;
};


class Mesh
{
	friend class MeshBuilder;
	friend class MeshSlicer;
	friend class BSPMeshSlicer;
	friend class FBXMeshBuilder;
	friend class BSP;
public:
	Mesh()
	{
	}

	~Mesh()
	{
	}

	Mesh(const Mesh& other)
	{
		polygons = other.polygons;
	}

	Mesh& operator = (const Mesh& other)
	{
		polygons = other.polygons;

		return *this;
	}

	void Flip()
	{
		for (size_t i = 0; i < polygons.size(); i++)
		{
			polygons[i].Flip();
		}
	}

	bool IsEmpty() const
	{
		return polygons.size() == 0;
	}

	friend Mesh Intersect(const Mesh& m0, const Mesh& m1)
	{
		Mesh result;
		/*
			var a = new CSG.Node(this.clone().polygons);
			var b = new CSG.Node(csg.clone().polygons);
			a.invert();
			b.clipTo(a);
			b.invert();
			a.clipTo(b);
			b.clipTo(a);
			a.build(b.allPolygons());
			a.invert();
			return CSG.fromPolygons(a.allPolygons());
		*/
		if (!m0.IsEmpty() && !m1.IsEmpty())
		{
			BSP a(m0.polygons);
			BSP b(m1.polygons);

			a.Invert();
			b.ClipBy(a);		// find polygon of b inside ~a
			b.Invert();
			a.ClipBy(b);		// find polygon of a inside ~b

			b.ClipBy(a);		// remove b polygons coplanar with a

			// join polygons in a, b
			// a.FromPolygons(b.ToPolygons());
			std::vector<Polygon> polygons;
			b.ToPolygons(polygons);
			a.FromPolygons(polygons);

			a.Invert();			// ~(~A | ~B)

			// output
			a.ToPolygons(result.polygons);
		}
		else if (m0.IsEmpty() && !m1.IsEmpty())
		{
			result = Mesh();
		}
		else if (!m0.IsEmpty() && m1.IsEmpty())
		{
			result = Mesh();
		}
		else// if (m0.IsEmpty() && m1.IsEmpty())
		{
			result = Mesh();
		}

		return result;
	}

	void Transform(const Vector3& t, const Vector3& s)
	{
		for (size_t i = 0; i < polygons.size(); i++)
		{
			polygons[i].Transform(t, s);
		}
	}

	void Add(const Polygon& polygon)
	{
		polygons.push_back(polygon);
	}
private:
	AABB aabb;
	std::vector<Polygon> polygons;
};

typedef std::vector<Mesh> MeshArray;

#endif