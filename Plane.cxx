#include "Plane.h"

Plane::Plane()
{
}

Plane::Plane(const Plane& other)
{
	this->n = other.n;
	this->d = other.d;
}

Plane::Plane(const Vector3& n, float d)
{
	this->n = n;
	this->d = d;
}

Plane::Plane(const Vector3& n, const Vector3& p)
{
	this->n = n;
	this->d = n.Dot(p);
}

Plane::Plane(const Vector3& p0, const Vector3& p1, const Vector3& p2)
{
	Vector3 e0 = p1 - p0;
	Vector3 e1 = p2 - p0;

	this->n = e0.UnitCross(e1);
	this->d = this->n.Dot(p0);
}

Plane& Plane::operator= (const Plane& other)
{
	this->n = other.n;
	this->d = other.d;

	return *this;
}

void Plane::Flip()
{
	n = -n;
	d = -d;
}

bool Plane::WhichSide(const Vector3& p) const
{
	float dist = DistanceTo(p);

	if (dist >= (float)0.0f)
		return true;
	else
		return false;
}

float Plane::DistanceTo(const Vector3& p) const
{
	return n.Dot(p) - d;
}

bool Plane::isCoplanar(const Plane& other) const
{
	return	fabs(n[0] - other.n[0]) < Math::ZeroTolerance &&
		fabs(n[1] - other.n[1]) < Math::ZeroTolerance &&
		fabs(n[2] - other.n[2]) < Math::ZeroTolerance &&
		fabs(d - other.d) < Math::ZeroTolerance;
}

const Vector3& Plane::Normal() const
{
	return n;
}

float Plane::D() const
{
	return d;
}