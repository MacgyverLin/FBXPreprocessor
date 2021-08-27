#include "Vertex.h"

Vertex::Vertex()
{
	this->position = Vector3::Zero;

	for (size_t i = 0; i < NUM_COLORS; i++)
		this->colors[i] = Color::Black;

	for (size_t i = 0; i < NUM_UVS; i++)
		this->uvs[i] = Vector2::Zero;

	for (size_t i = 0; i < NUM_NORMALS; i++)
		this->normals[i] = Vector3::Zero;

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		this->tangents[i] = Vector3::Zero;

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		this->binormals[i] = Vector3::Zero;

}

Vertex::Vertex(const Vector3& position, const Color& color, const Vector2& uv, const Vector3& normal, const Vector3& tangent, const Vector3& binormal)
{
	this->position = position;

	for (size_t i = 0; i < NUM_COLORS; i++)
		this->colors[i] = color;

	for (size_t i = 0; i < NUM_UVS; i++)
		this->uvs[i] = uv;

	for (size_t i = 0; i < NUM_NORMALS; i++)
		this->normals[i] = normal;

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		this->tangents[i] = tangent;

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		this->binormals[i] = binormal;
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
	position = other.position;

	for (size_t i = 0; i < NUM_COLORS; i++)
		colors[i] = other.colors[i];

	for (size_t i = 0; i < NUM_UVS; i++)
		uvs[i] = other.uvs[i];

	for (size_t i = 0; i < NUM_NORMALS; i++)
		normals[i] = other.normals[i];

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		tangents[i] = other.tangents[i];

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		binormals[i] = other.binormals[i];
}

Vertex& Vertex::operator = (const Vertex& other)
{
	position = other.position;

	for (size_t i = 0; i < NUM_COLORS; i++)
		colors[i] = other.colors[i];

	for (size_t i = 0; i < NUM_UVS; i++)
		uvs[i] = other.uvs[i];

	for (size_t i = 0; i < NUM_NORMALS; i++)
		normals[i] = other.normals[i];

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		tangents[i] = other.tangents[i];

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		binormals[i] = other.binormals[i];

	return *this;
}

int Vertex::CompareArrays(const Vertex& v) const
{
	return memcmp(this, &v, sizeof(Vertex));
}

bool Vertex::operator == (const Vertex& v) const
{
	return CompareArrays(v) == 0;
}

bool Vertex::operator != (const Vertex& v) const
{
	return CompareArrays(v) != 0;
}

bool Vertex::operator<  (const Vertex& v) const
{
	return CompareArrays(v) < 0;
}

bool Vertex::operator<= (const Vertex& v) const
{
	return CompareArrays(v) <= 0;
}

bool Vertex::operator>  (const Vertex& v) const
{
	return CompareArrays(v) > 0;
}

bool Vertex::operator>= (const Vertex& v) const
{
	return CompareArrays(v) >= 0;
}

// arithmetic operations
Vertex Vertex::operator+ (const Vertex& v) const
{
	Vertex result;

	result.position = position + v.position;

	for (size_t i = 0; i < NUM_COLORS; i++)
		result.colors[i] = colors[i] + v.colors[i];

	for (size_t i = 0; i < NUM_UVS; i++)
		result.uvs[i] = uvs[i] + v.uvs[i];

	for (size_t i = 0; i < NUM_NORMALS; i++)
		result.normals[i] = normals[i] + v.normals[i];

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		result.tangents[i] = tangents[i] + v.tangents[i];

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		result.binormals[i] = binormals[i] + v.binormals[i];

	return result;
}

Vertex Vertex::operator- (const Vertex& v) const
{
	Vertex result;

	result.position = position - v.position;

	for (size_t i = 0; i < NUM_COLORS; i++)
		result.colors[i] = colors[i] - v.colors[i];

	for (size_t i = 0; i < NUM_UVS; i++)
		result.uvs[i] = uvs[i] - v.uvs[i];

	for (size_t i = 0; i < NUM_NORMALS; i++)
		result.normals[i] = normals[i] - v.normals[i];

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		result.tangents[i] = tangents[i] - v.tangents[i];

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		result.binormals[i] = binormals[i] - v.binormals[i];

	return result;
}

Vertex Vertex::operator* (const Vertex& v) const
{
	Vertex result;

	result.position = position * v.position;

	for (size_t i = 0; i < NUM_COLORS; i++)
		result.colors[i] = colors[i] * v.colors[i];

	for (size_t i = 0; i < NUM_UVS; i++)
		result.uvs[i] = uvs[i] * v.uvs[i];

	for (size_t i = 0; i < NUM_NORMALS; i++)
		result.normals[i] = normals[i] * v.normals[i];

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		result.tangents[i] = tangents[i] * v.tangents[i];

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		result.binormals[i] = binormals[i] * v.binormals[i];

	return result;
}

Vertex Vertex::operator/ (const Vertex& v) const
{
	Vertex result;

	result.position = position / v.position;

	for (size_t i = 0; i < NUM_COLORS; i++)
		result.colors[i] = colors[i] / v.colors[i];

	for (size_t i = 0; i < NUM_UVS; i++)
		result.uvs[i] = uvs[i] / v.uvs[i];

	for (size_t i = 0; i < NUM_NORMALS; i++)
		result.normals[i] = normals[i] / v.normals[i];

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		result.tangents[i] = tangents[i] / v.tangents[i];

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		result.binormals[i] = binormals[i] / v.binormals[i];

	return result;
}

Vertex Vertex::operator* (float fScalar) const
{
	Vertex result;

	result.position = position * fScalar;

	for (size_t i = 0; i < NUM_COLORS; i++)
		result.colors[i] = colors[i] * fScalar;

	for (size_t i = 0; i < NUM_UVS; i++)
		result.uvs[i] = uvs[i] * fScalar;

	for (size_t i = 0; i < NUM_NORMALS; i++)
		result.normals[i] = normals[i] * fScalar;

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		result.tangents[i] = tangents[i] * fScalar;

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		result.binormals[i] = binormals[i] * fScalar;

	return result;
}

Vertex Vertex::operator/ (float fScalar) const
{
	Vertex result;

	result.position = position / fScalar;

	for (size_t i = 0; i < NUM_COLORS; i++)
		result.colors[i] = colors[i] / fScalar;

	for (size_t i = 0; i < NUM_UVS; i++)
		result.uvs[i] = uvs[i] / fScalar;

	for (size_t i = 0; i < NUM_NORMALS; i++)
		result.normals[i] = normals[i] / fScalar;

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		result.tangents[i] = tangents[i] / fScalar;

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		result.binormals[i] = binormals[i] / fScalar;

	return result;
}

Vertex Vertex::operator- () const
{
	Vertex result;

	result.position = -position;

	for (size_t i = 0; i < NUM_COLORS; i++)
		result.colors[i] = -colors[i];

	for (size_t i = 0; i < NUM_UVS; i++)
		result.uvs[i] = -uvs[i];

	for (size_t i = 0; i < NUM_NORMALS; i++)
		result.normals[i] = -normals[i];

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		result.tangents[i] = -tangents[i];

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		result.binormals[i] = -binormals[i];

	return result;
}

// arithmetic updates
Vertex& Vertex::operator+= (const Vertex& v)
{
	position += v.position;

	for (size_t i = 0; i < NUM_COLORS; i++)
		colors[i] += v.colors[i];

	for (size_t i = 0; i < NUM_UVS; i++)
		uvs[i] += v.uvs[i];

	for (size_t i = 0; i < NUM_NORMALS; i++)
		normals[i] += v.normals[i];

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		tangents[i] += v.tangents[i];

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		binormals[i] += v.binormals[i];

	return *this;
}

Vertex& Vertex::operator-= (const Vertex& v)
{
	position -= v.position;

	for (size_t i = 0; i < NUM_COLORS; i++)
		colors[i] -= v.colors[i];

	for (size_t i = 0; i < NUM_UVS; i++)
		uvs[i] -= v.uvs[i];

	for (size_t i = 0; i < NUM_NORMALS; i++)
		normals[i] -= v.normals[i];

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		tangents[i] -= v.tangents[i];

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		binormals[i] -= v.binormals[i];

	return *this;
}

Vertex& Vertex::operator*= (const Vertex& v)
{
	position *= v.position;

	for (size_t i = 0; i < NUM_COLORS; i++)
		colors[i] *= v.colors[i];

	for (size_t i = 0; i < NUM_UVS; i++)
		uvs[i] *= v.uvs[i];

	for (size_t i = 0; i < NUM_NORMALS; i++)
		normals[i] *= v.normals[i];

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		tangents[i] *= v.tangents[i];

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		binormals[i] *= v.binormals[i];

	return *this;
}

Vertex& Vertex::operator*= (float fScalar)
{
	position *= fScalar;

	for (size_t i = 0; i < NUM_COLORS; i++)
		colors[i] *= fScalar;

	for (size_t i = 0; i < NUM_UVS; i++)
		uvs[i] *= fScalar;

	for (size_t i = 0; i < NUM_NORMALS; i++)
		normals[i] *= fScalar;

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		tangents[i] *= fScalar;

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		binormals[i] *= fScalar;

	return *this;
}

Vertex& Vertex::operator/= (float fScalar)
{
	position /= fScalar;

	for (size_t i = 0; i < NUM_COLORS; i++)
		colors[i] /= fScalar;

	for (size_t i = 0; i < NUM_UVS; i++)
		uvs[i] /= fScalar;

	for (size_t i = 0; i < NUM_NORMALS; i++)
		normals[i] /= fScalar;

	for (size_t i = 0; i < NUM_TANGENTS; i++)
		tangents[i] /= fScalar;

	for (size_t i = 0; i < NUM_BINORMALS; i++)
		binormals[i] /= fScalar;

	return *this;
}

void Vertex::Flip()
{
	for (size_t i = 0; i < NUM_NORMALS; i++)
		normals[i] = -normals[i];
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