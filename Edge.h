#ifndef _Edge_h_
#define _Edge_h_

#include "AABB.h"

class Edge
{
public:
	Edge(int vertexIdx_ = -1, int startIdx_ = -1, int adjPolyIdx_ = -1)
	{
		vertexIdx = vertexIdx_;

		indices[0] = startIdx_;
		indices[1] = -1;

		adjacentPolygonIdx = adjPolyIdx_;
	}

	Edge& operator = (const Edge& other)
	{
		vertexIdx = other.vertexIdx;

		indices[0] = other.indices[0];
		indices[1] = other.indices[1];

		adjacentPolygonIdx = other.adjacentPolygonIdx;

		return *this;
	}

	int CompareArrays(const Edge& v) const
	{
		return memcmp(indices, v.indices, 2 * sizeof(indices[0]));
	}

	bool operator== (const Edge& v) const
	{
		return CompareArrays(v) == 0;
	}

	bool operator!= (const Edge& v) const
	{
		return CompareArrays(v) != 0;
	}

	bool operator<  (const Edge& v) const
	{
		return CompareArrays(v) < 0;
	}

	bool operator<= (const Edge& v) const
	{
		return CompareArrays(v) <= 0;
	}

	bool operator>  (const Edge& v) const
	{
		return CompareArrays(v) > 0;
	}

	bool operator>= (const Edge& v) const
	{
		return CompareArrays(v) >= 0;
	}

	int GetVertexIdx() const
	{
		return vertexIdx;
	}

	int GetStartIdx() const
	{
		return indices[0];
	}

	int GetEndIdx() const
	{
		return indices[1];
	}

	int GetAdjacentPolygonIdx() const
	{
		return adjacentPolygonIdx;
	}

	void SetStartIdx(int idx)
	{
		indices[0] = idx;
	}

	void SetEndIdx(int idx)
	{
		indices[1] = idx;
	}

	void SetAdjacentPolygonIdx(int idx)
	{
		adjacentPolygonIdx = idx;
	}

	void Flip()
	{
		std::swap(indices[0], indices[1]);
	}

	bool IsNormalized() const
	{
		return indices[0] < indices[1];
	}
private:
	int vertexIdx;
	int indices[2];
	int adjacentPolygonIdx;
};

#endif