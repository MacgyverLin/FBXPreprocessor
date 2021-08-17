#include "AABB.h"

AABB::AABB()
{
}

AABB::AABB(const Vector3& min, const Vector3& max)
{
	this->min = Min(min, max);
	this->max = Max(min, max);
}

Vector3 AABB::GetExtent() const
{
	return max - min;
}

AABB operator + (const AABB& a0, const AABB& a1)
{
	AABB result;
	result.min = Min(a0.min, a1.min);
	result.max = Max(a0.max, a1.max);

	return result;
}

AABB AABB::operator += (const AABB& aabb)
{
	this->min = Min(min, aabb.min);
	this->max = Max(max, aabb.max);

	return *this;
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

	return min + GetExtent() * Vector3(Math::RangeRandom(minv, maxv), Math::RangeRandom(minv, maxv), Math::RangeRandom(minv, maxv));
}

const Vector3& AABB::GetMin() const
{
	return min;
}

const Vector3& AABB::GetMax() const
{
	return max;
}