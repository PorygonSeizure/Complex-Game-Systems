#ifndef _UTILITY_NPC_H_
#define _UTILITY_NPC_H_

#include <map>

#include "BaseNPC.h"
#include "UtilityMaths.h"
#include "UtilityValue.h"
#include "UtilityScore.h"

using namespace UtilitySystem;

class UtilityNPC : public BaseNPC
{
public:
	UtilityNPC(World* world);
	virtual ~UtilityNPC();

protected:
	void SelectAction(float deltaTime) override;

private:
	void SetupValue(UtilityValue& utilityValue, std::string behaviourName, float value, UtilityScore* utilityScore);
	UtilityValue m_waterValue;
	UtilityValue m_foodValue;
	UtilityValue m_restValue;
	UtilityValue m_treeValue;
	UtilityValue m_houseValue;

	std::map<std::string, UtilityScore*> m_utilityScoreMap;

	UtilityScore* m_waterScore;
	UtilityScore* m_foodScore;
	UtilityScore* m_restScore;
	UtilityScore* m_treeScore;
	UtilityScore* m_houseScore;
};

#endif