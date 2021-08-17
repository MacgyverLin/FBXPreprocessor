#include "Color.h"

Color::Color()
{
}

Color::Color(const FbxVector4& v)
{
	this->m[0] = (float)v[0];
	this->m[1] = (float)v[1];
	this->m[2] = (float)v[2];
	this->m[3] = (float)v[3];
}

Color::Color(float r, float g, float b, float a)
{
	this->m[0] = r;
	this->m[1] = g;
	this->m[2] = b;
	this->m[3] = a;
}

Color::Color(const float* m)
{
	this->m[0] = m[0];
	this->m[1] = m[1];
	this->m[2] = m[2];
	this->m[3] = m[3];
}

Color::Color(const Color& v)
{
	this->m[0] = v.m[0];
	this->m[1] = v.m[1];
	this->m[2] = v.m[2];
	this->m[3] = v.m[3];
}

void Color::Set(float r, float g, float b, float a)
{
	this->m[0] = r;
	this->m[1] = g;
	this->m[2] = b;
	this->m[3] = a;
}

// coordinate access
Color::operator const float* () const
{
	return m;
}

Color::operator float* ()
{
	return m;
}

float Color::operator[] (int i) const
{
	return m[i];
}

float& Color::operator[] (int i)
{
	return m[i];
}

float Color::R() const
{
	return m[0];
}

float& Color::R()
{
	return m[0];
}

float Color::G() const
{
	return m[1];
}

float& Color::G()
{
	return m[1];
}

float Color::B() const
{
	return m[2];
}

float& Color::B()
{
	return m[2];
}

float Color::A() const
{
	return m[3];
}

float& Color::A()
{
	return m[3];
}

Color Min(const Color& v0, const Color& v1)
{
	return Color(std::min(v0[0], v1[0]), std::min(v0[1], v1[1]), std::min(v0[2], v1[2]), std::min(v0[3], v1[3]));
}

Color Max(const Color& v0, const Color& v1)
{
	return Color(std::max(v0[0], v1[0]), std::max(v0[1], v1[1]), std::max(v0[2], v1[2]), std::max(v0[3], v1[3]));
}

// assignment
Color& Color::operator= (const FbxColor& v)
{
	this->m[0] = (float)v[0];
	this->m[1] = (float)v[1];
	this->m[2] = (float)v[2];
	this->m[3] = (float)v[3];

	return *this;
}

Color& Color::operator= (const Color& v)
{
	this->m[0] = v.m[0];
	this->m[1] = v.m[1];
	this->m[2] = v.m[2];
	this->m[3] = v.m[3];

	return *this;
}

int Color::CompareArrays(const Color& v) const
{
	return memcmp(m, v.m, 4 * sizeof(float));
}

// comparison
bool Color::operator== (const Color& v) const
{
	return CompareArrays(v) == 0;
}

bool Color::operator!= (const Color& v) const
{
	return CompareArrays(v) != 0;
}

bool Color::operator<  (const Color& v) const
{
	return CompareArrays(v) < 0;
}

bool Color::operator<= (const Color& v) const
{
	return CompareArrays(v) <= 0;
}

bool Color::operator>  (const Color& v) const
{
	return CompareArrays(v) > 0;
}

bool Color::operator>= (const Color& v) const
{
	return CompareArrays(v) >= 0;
}

// arithmetic operations
Color Color::operator+ (const Color& v) const
{
	return Color
	(
		m[0] + v.m[0],
		m[1] + v.m[1],
		m[2] + v.m[2],
		m[3] + v.m[3]
	);
}

Color Color::operator- (const Color& v) const
{
	return Color
	(
		m[0] - v.m[0],
		m[1] - v.m[1],
		m[2] - v.m[2],
		m[3] - v.m[3]
	);
}

Color Color::operator* (const Color& v) const
{
	return Color
	(
		m[0] * v.m[0],
		m[1] * v.m[1],
		m[2] * v.m[2],
		m[3] * v.m[3]
	);
}

Color Color::operator/ (const Color& v) const
{
	return Color
	(
		m[0] / v.m[0],
		m[1] / v.m[1],
		m[2] / v.m[2],
		m[3] / v.m[3]
	);
}

Color Color::operator* (float fScalar) const
{
	return Color
	(
		m[0] * fScalar,
		m[1] * fScalar,
		m[2] * fScalar,
		m[3] * fScalar
	);
}

Color Color::operator/ (float fScalar) const
{
	return Color
	(
		m[0] / fScalar,
		m[1] / fScalar,
		m[2] / fScalar,
		m[3] / fScalar
	);
}

Color Color::operator- () const
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
Color& Color::operator+= (const Color& v)
{
	m[0] += v.m[0];
	m[1] += v.m[1];
	m[2] += v.m[2];
	m[3] += v.m[3];

	return *this;
}

Color& Color::operator-= (const Color& v)
{
	m[0] -= v.m[0];
	m[1] -= v.m[1];
	m[2] -= v.m[2];
	m[3] -= v.m[3];

	return *this;
}

Color& Color::operator*= (const Color& v)
{
	m[0] *= v.m[0];
	m[1] *= v.m[1];
	m[2] *= v.m[2];
	m[3] *= v.m[3];

	return *this;
}

Color& Color::operator*= (float fScalar)
{
	m[0] *= fScalar;
	m[1] *= fScalar;
	m[2] *= fScalar;
	m[3] *= fScalar;

	return *this;
}

Color& Color::operator/= (float fScalar)
{
	m[0] /= fScalar;
	m[1] /= fScalar;
	m[2] /= fScalar;
	m[3] /= fScalar;

	return *this;
}

// vector operations
float Color::Length() const
{
	return (float)sqrt(SquaredLength());
}

float Color::SquaredLength() const
{
	return m[0] * m[0] + m[1] * m[1] + m[2] * m[2] + m[3] * m[3];
}

float Color::Dot(const Color& v) const
{
	return m[0] * v.m[0] + m[1] * v.m[1] + m[2] * v.m[2] + m[3] * v.m[3];
}

float Color::Normalize()
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

Color Lerp(const Color& v0, const Color& v1, float t)
{
	return v0 * (1 - t) + v1 * t;
}