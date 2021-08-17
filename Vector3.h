#ifndef _Vector3_h_
#define _Vector3_h_

#include "Math.h"

///////////////////////////////////////////////////////////////////
class Vector3
{
public:
	Vector3();

	Vector3(const FbxVector4& v);

	Vector3(float fX, float fY, float fZ);

	Vector3(const float* m);

	Vector3(const Vector3& v);

	void Set(float fX, float fY, float fZ);

	// coordinate access
	operator const float* () const;

	operator float* ();

	float operator[] (int i) const;

	float& operator[] (int i);

	float X() const;

	float& X();

	float Y() const;

	float& Y();

	float Z() const;

	float& Z();

	// assignment
	Vector3& operator= (const FbxVector4& v);

	Vector3& operator= (const Vector3& v);

	int CompareArrays(const Vector3& v) const;

	// comparison
	friend Vector3 Min(const Vector3& v0, const Vector3& v1);

	friend Vector3 Max(const Vector3& v0, const Vector3& v1);

	bool operator== (const Vector3& v) const;

	bool operator!= (const Vector3& v) const;

	bool operator<  (const Vector3& v) const;

	bool operator<= (const Vector3& v) const;

	bool operator>  (const Vector3& v) const;

	bool operator>= (const Vector3& v) const;

	// arithmetic operations
	Vector3 operator+ (const Vector3& v) const;

	Vector3 operator- (const Vector3& v) const;

	Vector3 operator* (const Vector3& v) const;

	Vector3 operator/ (const Vector3& v) const;

	Vector3 operator* (float fScalar) const;

	Vector3 operator/ (float fScalar) const;

	Vector3 operator- () const;

	// arithmetic updates
	Vector3& operator+= (const Vector3& v);

	Vector3& operator-= (const Vector3& v);

	Vector3& operator*= (const Vector3& v);

	Vector3& operator*= (float fScalar);

	Vector3& operator/= (float fScalar);

	// vector operations
	float Length() const;

	float SquaredLength() const;

	float Dot(const Vector3& v) const;

	float Normalize();

	Vector3 Cross(const Vector3& v) const;

	Vector3 UnitCross(const Vector3& v) const;

	friend Vector3 Lerp(const Vector3& v0, const Vector3& v1, float t);
private:
	float m[3];
};

#endif