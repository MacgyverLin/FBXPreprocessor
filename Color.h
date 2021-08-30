#ifndef _Color_h_
#define _Color_h_

#include "Math.h"

///////////////////////////////////////////////////////////////////
class Color
{
public:
	Color();

	Color(const FbxVector4& v);

	Color(float r, float g, float b, float a);

	Color(const float* m);

	Color(const Color& v);

	void Set(float r, float g, float b, float a);

	// coordinate access
	operator const float* () const;

	operator float* ();

	float operator[] (int i) const;

	float& operator[] (int i);

	float R() const;

	float& R();

	float G() const;

	float& G();

	float B() const;

	float& B();

	float A() const;

	float& A();

	friend Color Min(const Color& v0, const Color& v1);

	friend Color Max(const Color& v0, const Color& v1);

	// assignment
	Color& operator= (const FbxColor& v);

	Color& operator= (const Color& v);

	int CompareArrays(const Color& v) const;

	// comparison
	bool operator== (const Color& v) const;

	bool operator!= (const Color& v) const;

	bool operator<  (const Color& v) const;

	bool operator<= (const Color& v) const;

	bool operator>  (const Color& v) const;

	bool operator>= (const Color& v) const;

	// arithmetic operations
	Color operator+ (const Color& v) const;

	Color operator- (const Color& v) const;

	Color operator* (const Color& v) const;

	Color operator/ (const Color& v) const;

	Color operator* (float fScalar) const;

	Color operator/ (float fScalar) const;

	Color operator- () const;

	friend Color operator* (float fScalar, const Color& v);

	// arithmetic updates
	Color& operator+= (const Color& v);

	Color& operator-= (const Color& v);
	
	Color& operator*= (const Color& v);

	Color& operator*= (float fScalar);

	Color& operator/= (float fScalar);

	// vector operations
	float Length() const;

	float SquaredLength() const;

	float Dot(const Color& v) const;

	float Normalize();

	friend Color Lerp(const Color& v0, const Color& v1, float t);

	static Color Black;
	static Color White;
private:
	float m[4];
};

#endif