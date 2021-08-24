#include "Edge.h"

//////////////////////////////////////////////////////
// comparison
Edge::Edge(int i0, int i1)
{
	indices[0] = std::min(i0, i1);
	indices[1] = std::max(i0, i1);
}

Edge::~Edge()
{
}

Edge::Edge(const Edge& other)
{
	indices[0] = other.indices[0];
	indices[1] = other.indices[1];
}

Edge& Edge::operator = (const Edge& other)
{
	indices[0] = other.indices[0];
	indices[1] = other.indices[1];

	return *this;
}

int Edge::operator [] (int i) const
{
	return indices[i];
}

int Edge::StartIdx() const
{
	return indices[0];
}

int Edge::EndIdx() const
{
	return indices[1];
}

int Edge::CompareArrays(const Edge& v) const
{
	return memcmp(indices, v.indices, 2 * sizeof(indices[0]));
}

bool Edge::operator== (const Edge& v) const
{
	return CompareArrays(v) == 0;
}

bool Edge::operator!= (const Edge& v) const
{
	return CompareArrays(v) != 0;
}

bool Edge::operator<  (const Edge& v) const
{
	return CompareArrays(v) < 0;
}

bool Edge::operator<= (const Edge& v) const
{
	return CompareArrays(v) <= 0;
}

bool Edge::operator>  (const Edge& v) const
{
	return CompareArrays(v) > 0;
}

bool Edge::operator>= (const Edge& v) const
{
	return CompareArrays(v) >= 0;
}

void Edge::Flip()
{
	std::swap(indices[0], indices[1]);
}