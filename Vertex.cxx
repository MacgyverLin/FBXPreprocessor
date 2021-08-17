#include "Vertex.h"

Vertex::Vertex()
{
}

Vertex::Vertex(const Vector3& position, const Vector3& normal, const Vector2& uv)
{
	this->position = position;

	for (size_t i = 0; i < NUM_COLORS; i++)
		this->colors[i] = Color(1.0f, 1.0f, 1.0f, 1.0f);

	for (size_t i = 0; i < NUM_UVS; i++)
		this->uvs[i] = uv;

	for (size_t i = 0; i < NUM_NORMALS; i++)
		this->normals[i] = normal;

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		this->tangents[i] = normal;

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		this->binormals[i] = normal;
}

Vertex::Vertex(const Vertex& other)
{
	memcpy(this, &other, sizeof(Vertex));
}

Vertex& Vertex::operator = (const Vertex& other)
{
	memcpy(this, &other, sizeof(Vertex));

	return *this;
}

Vertex Lerp(const Vertex& v0, const Vertex& v1, float t)
{
	Vertex result;

	result.position = Lerp(v0.position, v1.position, t);

	for (size_t i = 0; i < NUM_COLORS; i++)
		result.colors[i] = Lerp(v0.colors[i], v1.colors[i], t);

	for (size_t i = 0; i < NUM_UVS; i++)
		result.uvs[i] = Lerp(v0.uvs[i], v1.uvs[i], t);

	for (size_t i = 0; i < NUM_NORMALS; i++)
		result.normals[i] = Lerp(v0.normals[i], v1.normals[i], t);

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		result.tangents[i] = Lerp(v0.tangents[i], v1.tangents[i], t);

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		result.binormals[i] = Lerp(v0.binormals[i], v1.binormals[i], t);

	return result;
}