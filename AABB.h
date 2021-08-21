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

	Vector3& GetMin();

	Vector3& GetMax();
	
	const Vector3& GetMin() const;
	
	const Vector3& GetMax() const;

	AABB operator + (const AABB& other) const;
	
	AABB& operator += (const AABB& other);

	AABB operator + (const Vector3& v) const;

	AABB& operator += (const Vector3& v);

	Vector3 GetExtent() const;
	
	Vector3 GetMajorAxis(bool randomize = false) const;
	
	Vector3 GetMinorAxis(bool randomize = false) const;

	Vector3 GetCenter(bool randomize = false) const;
private:
	Vector3 minimun;
	Vector3 maximun;
};

#endif