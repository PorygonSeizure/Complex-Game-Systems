#ifndef UTILITY_MATH_H_
#define UTILITY_MATH_H_

#include <algorithm>

namespace UtilitySystem
{
	namespace UtilityMath
	{
		static float LinearNormalise(float min, float max, float value)
		{
			float campedValue = std::max(std::min(max, value), min);
			float range = max - min;
			return (campedValue - min) / (range);
		}

		static float QuadraticNormalise(float min, float max, float fValue, float fPower)
		{
			float campedValue = std::max(std::min(max, fValue), min);

			return pow(campedValue, fPower) / pow(max, fPower);
		}

	}
}

#endif