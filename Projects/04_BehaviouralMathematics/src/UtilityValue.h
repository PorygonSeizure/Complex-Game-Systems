#ifndef _UTILITY_VALUE_
#define _UTILITY_VALUE_

namespace UtilitySystem
{
	class UtilityValue
	{

	public:
		enum NormalizationType
		{
			LINEAR,
			INVERSE_LINEAR,
			QUADRATIC,
			INVERSE_QUADRATIC
		};

		UtilityValue() : UtilityValue(LINEAR, 0, 1) {}
		UtilityValue(NormalizationType type, float min, float max) : m_min(min), m_max(max), m_value(min), m_normalizedValue(0), m_normalizationType(type) {}
		~UtilityValue() {}

		void SetMinMaxValues(float min, float max);

		void SetNormalizationType(NormalizationType type) { m_normalizationType = type; }
		void SetQuadraticPower(float power) { m_power = power; }

		void SetValue(float value) { m_value = value; }

		float Evaluate();
	private:
		float m_min;
		float m_max;
		float m_power; //Only used when using Quadratic Normalization

		float m_value;
		float m_normalizedValue;

		NormalizationType m_normalizationType;
	};
}


#endif