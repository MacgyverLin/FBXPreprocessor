#ifndef _Matrix4_h_
#define _Matrix4_h_

#include "Vector2.h"
#include "Vector3.h"

class Matrix4
{
public:
	Matrix4();

	~Matrix4();

	Matrix4(const Vector3& axis, float angle);

	Matrix4(const Vector3& col0, const Vector3& col1, const Vector3& col2, const Vector3& col3);

	void InitZero();

	void InitIdentity();

	void Init(const Vector3& col0, const Vector3& col1, const Vector3& col2, const Vector3& col3);

	void InitRotateAxisAngle(const Vector3& axis, float angle);

	operator const float* () const;

	operator float* ();

	const float* operator[] (int iRow) const;

	float* operator[] (int iRow);

	float operator() (int iRow, int iCol) const;

	float& operator() (int iRow, int iCol);

	int I(int iRow, int iCol) const;

	Matrix4 Inverse() const;

	Vector3 TimesPositionVector(const Vector3& v) const;

	Vector3 TimesDirectionVector(const Vector3& v) const;
private:
private:
	float m[16];
};

#endif