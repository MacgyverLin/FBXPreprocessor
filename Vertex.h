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
	friend class MeshBuilder;
	friend class MeshSlicer;
	friend class BSPMeshSlicer;
	friend class FBXMeshBuilder;
	friend class Polygon;
	friend class BSP;
public:
	Vertex();

	Vertex(const Vector3& position, const Vector3& normal, const Vector2& uv);

	Vertex(const Vertex& other);

	Vertex& operator = (const Vertex& other);

	friend Vertex Lerp(const Vertex& v0, const Vertex& v1, float t);
private:
	Vector3 position;
	Color colors[NUM_COLORS];
	Vector2 uvs[NUM_UVS];
	Vector3 normals[NUM_NORMALS];
	Vector3 tangents[NUM_TANGENTS];
	Vector3 binormals[NUM_BINORMALS];
};

#endif