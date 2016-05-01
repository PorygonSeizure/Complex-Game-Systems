#ifndef _UTILITY_SCORE_H_
#define _UTILITY_SCORE_H_

#include <vector>

namespace UtilitySystem
{
	class UtilityValue;

	class UtilityScore
	{
	public:
		UtilityScore() {};
		~UtilityScore() {};

		void AddUtilityValue(UtilityValue* value, float modifier);

		float GetUtilityScore();
	private:
		struct UtilityInfo
		{
			UtilityValue* value;
			float modifier;
		};
		std::vector<UtilityInfo> m_utilityValues;
	};

}

#endif