#include "UtilityValue.h"
#include "UtilityMaths.h"

#include <assert.h>
#include <algorithm>

namespace UtilitySystem
{
	void UtilityValue::SetMinMaxValues(float min, float max)
	{
		m_min = min;
		m_max = max;
	}

	float UtilityValue::Evaluate()
	{
		switch (m_normalizationType)
		{
		case UtilityValue::LINEAR:
			m_normalizedValue = UtilityMath::LinearNormalise(m_min, m_max, m_value);
			break;
		case UtilityValue::INVERSE_LINEAR:
			m_normalizedValue = 1.f - UtilityMath::LinearNormalise(m_min, m_max, m_value);
			break;
		case UtilityValue::QUADRATIC:
			m_normalizedValue = UtilityMath::QuadraticNormalise(m_min, m_max, m_value, m_power);
			break;
		case UtilityValue::INVERSE_QUADRATIC:
			m_normalizedValue = 1.f - UtilityMath::QuadraticNormalise(m_min, m_max, m_value, m_power);
			break;
		default:
			assert(false && "No Utility Function Set");
			break;
		}

		m_normalizedValue = std::max(std::min(m_normalizedValue, 1.f), 0.f);
		return m_normalizedValue;
	}

}


