#ifndef _Vector2_h_
#define _Vector2_h_

#include "Math.h"

class Vector2
{
public:
	Vector2();

	Vector2(const FbxVector4& v);

	Vector2(const FbxVector2& v);

	Vector2(float fX, float fY);

	Vector2(const float* m);

	Vector2(const Vector2& v);

	void Set(float fX, float fY);

	// coordinate access
	operator const float* () const;

	operator float* ();

	float operator[] (int i) const;

	float& operator[] (int i);

	float X() const;

	float& X();

	float Y() const;

	float& Y();

	// assignment
	Vector2& operator= (const FbxVector2& v);

	Vector2& operator= (const FbxVector4& v);

	Vector2& operator= (const Vector2& v);

	int CompareArrays(const Vector2& v) const;

	// comparison
	bool operator== (const Vector2& v) const;

	bool operator!= (const Vector2& v) const;

	bool operator<  (const Vector2& v) const;

	bool operator<= (const Vector2& v) const;

	bool operator>  (const Vector2& v) const;

	bool operator>= (const Vector2& v) const;

	// arithmetic operations
	Vector2 operator+ (const Vector2& v) const;

	Vector2 operator- (const Vector2& v) const;

	Vector2 operator* (const Vector2& v) const;

	Vector2 operator/ (const Vector2& v) const;

	Vector2 operator* (float fScalar) const;

	Vector2 operator/ (float fScalar) const;

	Vector2 operator- () const;

	// arithmetic updates
	Vector2& operator+= (const Vector2& v);

	Vector2& operator-= (const Vector2& v);

	Vector2& operator*= (const Vector2& v);

	Vector2& operator*= (float fScalar);

	Vector2& operator/= (float fScalar);

	// vector operations
	friend Vector2 Min(const Vector2& v0, const Vector2& v1);

	friend Vector2 Max(const Vector2& v0, const Vector2& v1);

	float Length() const;

	float SquaredLength() const;

	float Dot(const Vector2& v) const;

	float Normalize();

	Vector2 Perp() const;

	Vector2 UnitPerp() const;

	friend Vector2 Lerp(const Vector2& v0, const Vector2& v1, float t);
private:
	float m[2];
};

#endif