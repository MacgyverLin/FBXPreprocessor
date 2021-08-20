#include "Vector3.h"

Vector3 Vector3::Zero(0.0f, 0.0f, 0.0f);
Vector3 Vector3::XAxis(1.0f, 0.0f, 0.0f);
Vector3 Vector3::YAxis(0.0f, 1.0f, 0.0f);
Vector3 Vector3::ZAxis(0.0f, 0.0f, 1.0f);

///////////////////////////////////////////////////////////////////
Vector3::Vector3()
{
}

Vector3::Vector3(const FbxVector4& v)
{
	this->m[0] = (float)v[0];
	this->m[1] = (float)v[1];
	this->m[2] = (float)v[2];
}

Vector3::Vector3(float fX, float fY, float fZ)
{
	this->m[0] = fX;
	this->m[1] = fY;
	this->m[2] = fZ;
}

Vector3::Vector3(const float* m)
{
	this->m[0] = m[0];
	this->m[1] = m[1];
	this->m[2] = m[2];
}

Vector3::Vector3(const Vector3& v)
{
	this->m[0] = v.m[0];
	this->m[1] = v.m[1];
	this->m[2] = v.m[2];
}

void Vector3::Set(float fX, float fY, float fZ)
{
	this->m[0] = fX;
	this->m[1] = fY;
	this->m[2] = fZ;
}

// coordinate access
Vector3::operator const float* () const
{
	return m;
}

Vector3::operator float* ()
{
	return m;
}

float Vector3::operator[] (int i) const
{
	return m[i];
}

float& Vector3::operator[] (int i)
{
	return m[i];
}

float Vector3::X() const
{
	return m[0];
}

float& Vector3::X()
{
	return m[0];
}

float Vector3::Y() const
{
	return m[1];
}

float& Vector3::Y()
{
	return m[1];
}

float Vector3::Z() const
{
	return m[2];
}

float& Vector3::Z()
{
	return m[2];
}

// assignment
Vector3& Vector3::operator= (const FbxVector4& v)
{
	this->m[0] = (float)v[0];
	this->m[1] = (float)v[1];
	this->m[2] = (float)v[2];

	return *this;
}

Vector3& Vector3::operator= (const Vector3& v)
{
	this->m[0] = v.m[0];
	this->m[1] = v.m[1];
	this->m[2] = v.m[2];

	return *this;
}

int Vector3::CompareArrays(const Vector3& v) const
{
	return memcmp(m, v.m, 3 * sizeof(float));
}

// comparison
Vector3 Min(const Vector3& v0, const Vector3& v1)
{
	return Vector3(std::min(v0[0], v1[0]), std::min(v0[1], v1[1]), std::min(v0[2], v1[2]));
}

Vector3 Max(const Vector3& v0, const Vector3& v1)
{
	return Vector3(std::max(v0[0], v1[0]), std::max(v0[1], v1[1]), std::max(v0[2], v1[2]));
}

bool Vector3::operator== (const Vector3& v) const
{
	return CompareArrays(v) == 0;
}

bool Vector3::operator!= (const Vector3& v) const
{
	return CompareArrays(v) != 0;
}

bool Vector3::operator<  (const Vector3& v) const
{
	return CompareArrays(v) < 0;
}

bool Vector3::operator<= (const Vector3& v) const
{
	return CompareArrays(v) <= 0;
}

bool Vector3::operator>  (const Vector3& v) const
{
	return CompareArrays(v) > 0;
}

bool Vector3::operator>= (const Vector3& v) const
{
	return CompareArrays(v) >= 0;
}

// arithmetic operations
Vector3 Vector3::operator+ (const Vector3& v) const
{
	return Vector3
	(
		m[0] + v.m[0],
		m[1] + v.m[1],
		m[2] + v.m[2]
	);
}

Vector3 Vector3::operator- (const Vector3& v) const
{
	return Vector3
	(
		m[0] - v.m[0],
		m[1] - v.m[1],
		m[2] - v.m[2]
	);
}

Vector3 Vector3::operator* (const Vector3& v) const
{
	return Vector3
	(
		m[0] * v.m[0],
		m[1] * v.m[1],
		m[2] * v.m[2]
	);
}

Vector3 Vector3::operator/ (const Vector3& v) const
{
	return Vector3
	(
		m[0] / v.m[0],
		m[1] / v.m[1],
		m[2] / v.m[2]
	);
}

Vector3 Vector3::operator* (float fScalar) const
{
	return Vector3
	(
		m[0] * fScalar,
		m[1] * fScalar,
		m[2] * fScalar
	);
}

Vector3 Vector3::operator/ (float fScalar) const
{
	return Vector3
	(
		m[0] / fScalar,
		m[1] / fScalar,
		m[2] / fScalar
	);
}

Vector3 Vector3::operator- () const
{
	return Vector3
	(
		-m[0],
		-m[1],
		-m[2]
	);
}

// arithmetic updates
Vector3& Vector3::operator+= (const Vector3& v)
{
	m[0] += v.m[0];
	m[1] += v.m[1];
	m[2] += v.m[2];

	return *this;
}

Vector3& Vector3::operator-= (const Vector3& v)
{
	m[0] -= v.m[0];
	m[1] -= v.m[1];
	m[2] -= v.m[2];

	return *this;
}

Vector3& Vector3::operator*= (const Vector3& v)
{
	m[0] *= v.m[0];
	m[1] *= v.m[1];
	m[2] *= v.m[2];

	return *this;
}

Vector3& Vector3::operator*= (float fScalar)
{
	m[0] *= fScalar;
	m[1] *= fScalar;
	m[2] *= fScalar;

	return *this;
}

Vector3& Vector3::operator/= (float fScalar)
{
	m[0] /= fScalar;
	m[1] /= fScalar;
	m[2] /= fScalar;

	return *this;
}

// vector operations
float Vector3::Length() const
{
	return (float)sqrt(SquaredLength());
}

float Vector3::SquaredLength() const
{
	return m[0] * m[0] + m[1] * m[1] + m[2] * m[2];
}

float Vector3::Dot(const Vector3& v) const
{
	return m[0] * v.m[0] + m[1] * v.m[1] + m[2] * v.m[2];
}

float Vector3::Normalize()
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

Vector3 Vector3::Cross(const Vector3& v) const
{
	return Vector3(
		m[1] * v.m[2] - m[2] * v.m[1],
		m[2] * v.m[0] - m[0] * v.m[2],
		m[0] * v.m[1] - m[1] * v.m[0]);
}

Vector3 Vector3::UnitCross(const Vector3& v) const
{
	Vector3 result = Cross(v);
	result.Normalize();

	return result;
}

Vector3 Lerp(const Vector3& v0, const Vector3& v1, float t)
{
	return v0 * (1 - t) + v1 * t;
}