#include "Vector2.h"

Vector2 Vector2::Zero(0.0f, 0.0f);
Vector2 Vector2::UnitX(1.0f, 0.0f);
Vector2 Vector2::UnitY(0.0f, 1.0f);

Vector2::Vector2()
{
}

Vector2::Vector2(const FbxVector4& v)
{
	this->m[0] = (float)v[0];
	this->m[1] = (float)v[1];
}

Vector2::Vector2(const FbxVector2& v)
{
	this->m[0] = (float)v[0];
	this->m[1] = (float)v[1];
}

Vector2::Vector2(float fX, float fY)
{
	this->m[0] = fX;
	this->m[1] = fY;
}

Vector2::Vector2(const float* m)
{
	this->m[0] = m[0];
	this->m[1] = m[1];
}

Vector2::Vector2(const Vector2& v)
{
	this->m[0] = v.m[0];
	this->m[1] = v.m[1];
}

void Vector2::Set(float fX, float fY)
{
	this->m[0] = fX;
	this->m[1] = fY;
}

// coordinate access
Vector2::operator const float* () const
{
	return m;
}

Vector2::operator float* ()
{
	return m;
}

float Vector2::operator[] (int i) const
{
	return m[i];
}

float& Vector2::operator[] (int i)
{
	return m[i];
}

float Vector2::X() const
{
	return m[0];
}

float& Vector2::X()
{
	return m[0];
}

float Vector2::Y() const
{
	return m[1];
}

float& Vector2::Y()
{
	return m[1];
}

// assignment
Vector2& Vector2::operator= (const FbxVector2& v)
{
	this->m[0] = (float)v[0];
	this->m[1] = (float)v[1];

	return *this;
}

Vector2& Vector2::operator= (const FbxVector4& v)
{
	this->m[0] = (float)v[0];
	this->m[1] = (float)v[1];

	return *this;
}

Vector2& Vector2::operator= (const Vector2& v)
{
	this->m[0] = v.m[0];
	this->m[1] = v.m[1];

	return *this;
}

int Vector2::CompareArrays(const Vector2& v) const
{
	return memcmp(m, v.m, 2 * sizeof(float));
}

// comparison
bool Vector2::operator== (const Vector2& v) const
{
	return CompareArrays(v) == 0;
}

bool Vector2::operator!= (const Vector2& v) const
{
	return CompareArrays(v) != 0;
}

bool Vector2::operator<  (const Vector2& v) const
{
	return CompareArrays(v) < 0;
}

bool Vector2::operator<= (const Vector2& v) const
{
	return CompareArrays(v) <= 0;
}

bool Vector2::operator>  (const Vector2& v) const
{
	return CompareArrays(v) > 0;
}

bool Vector2::operator>= (const Vector2& v) const
{
	return CompareArrays(v) >= 0;
}

// arithmetic operations
Vector2 Vector2::operator+ (const Vector2& v) const
{
	return Vector2
	(
		m[0] + v.m[0],
		m[1] + v.m[1]
	);
}

Vector2 Vector2::operator- (const Vector2& v) const
{
	return Vector2
	(
		m[0] - v.m[0],
		m[1] - v.m[1]
	);
}

Vector2 Vector2::operator* (const Vector2& v) const
{
	return Vector2
	(
		m[0] * v.m[0],
		m[1] * v.m[1]
	);
}

Vector2 Vector2::operator/ (const Vector2& v) const
{
	return Vector2
	(
		m[0] / v.m[0],
		m[1] / v.m[1]
	);
}

Vector2 Vector2::operator* (float fScalar) const
{
	return Vector2
	(
		m[0] * fScalar,
		m[1] * fScalar
	);
}

Vector2 Vector2::operator/ (float fScalar) const
{
	return Vector2
	(
		m[0] / fScalar,
		m[1] / fScalar
	);
}

Vector2 Vector2::operator- () const
{
	return Vector2
	(
		-m[0],
		-m[1]
	);
}

// arithmetic updates
Vector2& Vector2::operator+= (const Vector2& v)
{
	m[0] += v.m[0];
	m[1] += v.m[1];

	return *this;
}

Vector2& Vector2::operator-= (const Vector2& v)
{
	m[0] -= v.m[0];
	m[1] -= v.m[1];

	return *this;
}

Vector2& Vector2::operator*= (const Vector2& v)
{
	m[0] *= v.m[0];
	m[1] *= v.m[1];

	return *this;
}

Vector2& Vector2::operator*= (float fScalar)
{
	m[0] *= fScalar;
	m[1] *= fScalar;

	return *this;
}

Vector2& Vector2::operator/= (float fScalar)
{
	m[0] /= fScalar;
	m[1] /= fScalar;

	return *this;
}

// vector operations
Vector2 Min(const Vector2& v0, const Vector2& v1)
{
	return Vector2(std::min(v0[0], v1[0]), std::min(v0[1], v1[1]));
}

Vector2 Max(const Vector2& v0, const Vector2& v1)
{
	return Vector2(std::max(v0[0], v1[0]), std::max(v0[1], v1[1]));
}

float Vector2::Length() const
{
	return (float)sqrt(SquaredLength());
}

float Vector2::SquaredLength() const
{
	return m[0] * m[0] + m[1] * m[1];
}

float Vector2::Dot(const Vector2& v) const
{
	return m[0] * v.m[0] + m[1] * v.m[1];
}

float Vector2::Normalize()
{
	float fLength = Length();

	if (fLength > Math::Epsilon)
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

Vector2 Vector2::Perp() const
{
	return Vector2(m[1], -m[0]);
}

Vector2 Vector2::UnitPerp() const
{
	Vector2 result = Perp();
	result.Normalize();

	return result;
}

Vector2 Lerp(const Vector2& v0, const Vector2& v1, float t)
{
	return v0 * (1 - t) + v1 * t;
}