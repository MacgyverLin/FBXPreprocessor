#ifndef _AABB_h_
#define _AABB_h_

#include "Math.h"
#include "Vector2.h"
#include "Vector3.h"

class AABB
{
public:
	AABB();
	AABB(const Vector3& min, const Vector3& max);

	friend AABB operator + (const AABB& a0, const AABB& a1);
	AABB operator += (const AABB& aabb);

	Vector3 GetExtent() const;
	Vector3 GetMajorAxis(bool randomize = false) const;
	Vector3 GetCenter(bool randomize = false) const;
	const Vector3& GetMin() const;
	const Vector3& GetMax() const;
private:
	Vector3 min;
	Vector3 max;
};

#endif