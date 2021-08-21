#ifndef _Vertex_h_
#define _Vertex_h_

#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"

#define NUM_COLORS 1
#define NUM_UVS 8
#define NUM_NORMALS 1
#define NUM_TANGENTS 1
#define NUM_BINORMALS 1

class Vertex
{
public:
	Vertex();

	Vertex(const Vector3& position, const Color& color, const Vector2& uv, const Vector3& normal, const Vector3& tangent, const Vector3& binormal);

	Vertex(const Vector3& position, const Vector3& normal, const Vector2& uv);

	Vertex(const Vertex& other);

	// assignment
	Vertex& operator = (const Vertex& other);

	// comparison
	int CompareArrays(const Vertex& v) const;

	bool operator== (const Vertex& v) const;

	bool operator!= (const Vertex& v) const;

	bool operator<  (const Vertex& v) const;

	bool operator<= (const Vertex& v) const;

	bool operator>  (const Vertex& v) const;

	bool operator>= (const Vertex& v) const;

	// arithmetic operations
	Vertex operator+ (const Vertex& v) const;

	Vertex operator- (const Vertex& v) const;

	Vertex operator* (const Vertex& v) const;

	Vertex operator/ (const Vertex& v) const;

	Vertex operator* (float fScalar) const;

	Vertex operator/ (float fScalar) const;

	Vertex operator- () const;

	// arithmetic updates
	Vertex& operator+= (const Vertex& v);

	Vertex& operator-= (const Vertex& v);

	Vertex& operator*= (const Vertex& v);

	Vertex& operator*= (float fScalar);

	Vertex& operator/= (float fScalar);

	void Flip();

	friend Vertex Lerp(const Vertex& v0, const Vertex& v1, float t);

	Vector3 position;
	Color colors[NUM_COLORS];
	Vector2 uvs[NUM_UVS];
	Vector3 normals[NUM_NORMALS];
	Vector3 tangents[NUM_TANGENTS];
	Vector3 binormals[NUM_BINORMALS];
};

#endif