#ifndef _Math_h_
#define _Math_h_

#include "Common.h"
#include <vector>
#include <map>
#include <assert.h>

///////////////////////////////////////////////////////////////////
class Math
{
public:
	static void RandSeed(int seed = 0);
	static float UnitRandom();
	static float RangeRandom(float min, float max);
	static const float ZeroTolerance;
};

#define NUM_VERTICES_PER_POLYGON 3

#endif