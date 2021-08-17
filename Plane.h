#ifndef _Plane_h_
#define _Plane_h_

#include "Vector3.h"

class Plane
{
public:
	Plane();

	Plane(const Plane& other);

	Plane(const Vector3& n, float d);

	Plane(const Vector3& n, const Vector3& p);

	Plane(const Vector3& p0, const Vector3& p1, const Vector3& p2);

	Plane& operator= (const Plane& other);
	
	void Flip();

	bool WhichSide(const Vector3& p) const;

	float DistanceTo(const Vector3& p) const;

	bool isCoplanar(const Plane& other) const;

	const Vector3& Normal() const;

	float D() const;
private:
	Vector3 n;
	float d;
};

#endif