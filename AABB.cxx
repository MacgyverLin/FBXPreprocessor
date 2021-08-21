#include "AABB.h"

AABB::AABB()
{
}

AABB::AABB(const Vector3& min, const Vector3& max)
{
	minimun = Min(min, max);
	maximun = Max(min, max);
}

Vector3& AABB::GetMin()
{
	return minimun;
}

Vector3& AABB::GetMax()
{
	return maximun;
}

const Vector3& AABB::GetMin() const
{
	return minimun;
}

const Vector3& AABB::GetMax() const
{
	return maximun;
}

Vector3 AABB::GetExtent() const
{
	return maximun - minimun;
}

AABB AABB::operator + (const AABB& other) const
{
	return AABB(Min(minimun, other.minimun), Max(maximun, other.maximun));
}

AABB& AABB::operator += (const AABB& aabb)
{
	minimun = Min(minimun, aabb.minimun);
	maximun = Max(maximun, aabb.maximun);

	return *this;
}

AABB AABB::operator + (const Vector3& v) const
{
	return AABB(Min(minimun, v), Max(maximun, v));
}

AABB& AABB::operator += (const Vector3& v)
{
	minimun = Min(minimun, v);
	maximun = Max(maximun, v);

	return *this;
}

Vector3 AABB::GetMinorAxis(bool randomize) const
{
	Vector3 extent(GetExtent());

	float maxv = randomize ? 0.3f : 0.0f;

	Vector3 result;
	if (extent.X() < extent.Y() && extent.X() < extent.Z()) // X
		result = Vector3(1.0, Math::RangeRandom(0, maxv), Math::RangeRandom(0, maxv));
	else if (extent.Y() < extent.X() && extent.Y() < extent.Z()) // Y
		result = Vector3(Math::RangeRandom(0, maxv), 1.0, Math::RangeRandom(0, maxv));
	else // Z
		result = Vector3(Math::RangeRandom(0, maxv), Math::RangeRandom(0, maxv), 1.0);

	result.Normalize();

	return result;
}

Vector3 AABB::GetMajorAxis(bool randomize) const
{
	Vector3 extent(GetExtent());

	float maxv = randomize ? 0.3f : 0.0f;

	Vector3 result;
	if (extent.X() > extent.Y() && extent.X() > extent.Z()) // X
		result = Vector3(1.0, Math::RangeRandom(0, maxv), Math::RangeRandom(0, maxv));
	else if (extent.Y() > extent.X() && extent.Y() > extent.Z()) // Y
		result = Vector3(Math::RangeRandom(0, maxv), 1.0, Math::RangeRandom(0, maxv));
	else // Z
		result = Vector3(Math::RangeRandom(0, maxv), Math::RangeRandom(0, maxv), 1.0);

	result.Normalize();

	return result;
}

Vector3 AABB::GetCenter(bool randomize) const
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

	return minimun + GetExtent() * Vector3(Math::RangeRandom(minv, maxv), Math::RangeRandom(minv, maxv), Math::RangeRandom(minv, maxv));
}