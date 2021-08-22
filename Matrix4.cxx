#include "Matrix4.h"

Matrix4::Matrix4()
{
}

Matrix4::~Matrix4()
{
}

Matrix4::Matrix4(const Vector3& axis, float angle)
{
	InitRotateAxisAngle(axis, angle);
}

Matrix4::Matrix4(const Vector3& col0, const Vector3& col1, const Vector3& col2, const Vector3& col3)
{
	Init(col0, col1, col2, col3);
}

void Matrix4::InitZero()
{
	m[0] = 0.0f;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;

	m[4] = 0.0f;
	m[5] = 0.0f;
	m[6] = 0.0f;
	m[7] = 0.0f;

	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = 0.0f;
	m[11] = 0.0f;

	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 0.0f;
}

void Matrix4::InitIdentity()
{
	m[0] = 1.0f;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;

	m[4] = 0.0f;
	m[5] = 1.0f;
	m[6] = 0.0f;
	m[7] = 0.0f;

	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = 1.0f;
	m[11] = 0.0f;

	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}

void Matrix4::Init(const Vector3& col0, const Vector3& col1, const Vector3& col2, const Vector3& col3)
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

	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}

void Matrix4::InitRotateAxisAngle(const Vector3& axis, float angle)
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
	m[3] = 0.0f;

	m[4] = fXYM - fZSin;
	m[5] = fY2 * fOneMinusCos + fCos;
	m[6] = fYZM + fXSin;
	m[7] = 0.0f;

	m[8] = fXZM + fYSin;
	m[9] = fYZM - fXSin;
	m[10] = fZ2 * fOneMinusCos + fCos;
	m[11] = 0.0f;

	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}

Matrix4::operator const float* () const
{
	return m;
}

Matrix4::operator float* ()
{
	return m;
}

const float* Matrix4::operator[] (int iRow) const
{
	return &m[4 * iRow];
}

float* Matrix4::operator[] (int iRow)
{
	return &m[4 * iRow];
}

float Matrix4::operator() (int iRow, int iCol) const
{
	return m[I(iRow, iCol)];
}

float& Matrix4::operator() (int iRow, int iCol)
{
	return m[I(iRow, iCol)];
}

int Matrix4::I(int iRow, int iCol) const
{
	assert(0 <= iRow && iRow < 4 && 0 <= iCol && iCol < 4);

	return iCol + 4 * iRow;
}

Matrix4 Matrix4::Inverse() const
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
	if (abs(fDet) <= Math::Epsilon)
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

	float fInvDet = ((float)1.0f) / fDet;
	for (int iRow = 0; iRow < 4; iRow++)
	{
		for (int iCol = 0; iCol < 4; iCol++)
		{
			kInv[iRow][iCol] *= fInvDet;
		}
	}

	return kInv;
}

Vector3 Matrix4::TimesPositionVector(const Vector3& v) const
{
	Vector3 result;
	result[0] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
	result[1] = m[4] * v[0] + m[5] * v[1] + m[6] * v[2] + m[7];
	result[2] = m[8] * v[0] + m[9] * v[1] + m[10] * v[2] + m[11];

	return result;
}

Vector3 Matrix4::TimesDirectionVector(const Vector3& v) const
{
	Vector3 result;
	result[0] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2];
	result[1] = m[4] * v[0] + m[5] * v[1] + m[6] * v[2];
	result[2] = m[8] * v[0] + m[9] * v[1] + m[10] * v[2];

	return result;
}