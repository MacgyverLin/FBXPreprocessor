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

const float Math::Epsilon = FLT_EPSILON;
const float Math::ZeroTolerance = 1e-4f;