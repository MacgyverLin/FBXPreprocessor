#ifndef _Edge_h_
#define _Edge_h_

#include "AABB.h"
#include <functional>

class Edge
{
public:
	Edge(int i0 = -1, int i1 = -1);

	~Edge();

	Edge(const Edge& other);

	Edge& operator = (const Edge& other);

	int operator [] (int i) const;

	int StartIdx() const;

	int EndIdx() const;

	int CompareArrays(const Edge& v) const;

	bool operator== (const Edge& v) const;

	bool operator!= (const Edge& v) const;

	bool operator<  (const Edge& v) const;

	bool operator<= (const Edge& v) const;

	bool operator>  (const Edge& v) const;

	bool operator>= (const Edge& v) const;

	void Flip();
private:
	int indices[2];
};

#endif