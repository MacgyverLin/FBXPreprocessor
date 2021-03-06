#include "Math.h"

///////////////////////////////////////////////////////////////////
void Math::RandSeed(int seed)
{
	srand(seed);
}

float Math::UnitRandom()
{
	return ((float)rand()) / RAND_MAX;
}

float Math::RangeRandom(float min, float max)
{
	return UnitRandom() * (max - min) + min;
}

float Math::Max(float v0, float v1)
{
	return std::max(v0, v1);
}

float Math::Min(float v0, float v1)
{
	return std::min(v0, v1);
}

const float Math::Epsilon = FLT_EPSILON;
const float Math::ZeroTolerance = 1e-4f;