#include "UtilityScore.h"
#include "UtilityValue.h"

namespace UtilitySystem
{
	void UtilityScore::AddUtilityValue(UtilityValue* value, float modifier)
	{
		m_utilityValues.push_back({ value, modifier });
	}

	float UtilityScore::GetUtilityScore()
	{
		float score = 0.f;
		for (auto info : m_utilityValues)
		{
			if (score > 0)
				score *= (info.value->Evaluate() * info.modifier);
			else
				score = (info.value->Evaluate() * info.modifier);
		}

		return score;
	}
}