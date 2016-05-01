#include "UtilityNPC.h"
#include "World.h"

using namespace UtilitySystem;

UtilityNPC::UtilityNPC(World* world) : BaseNPC(world)
{
	m_waterScore = new UtilityScore();
	m_foodScore = new UtilityScore();
	m_restScore = new UtilityScore();
	m_treeScore = new UtilityScore();
	m_houseScore = new UtilityScore();
	SetupValue(m_waterValue, "collectWater", (float)GetWaterValue(), m_waterScore);
	SetupValue(m_foodValue, "collectFood", (float)GetFoodValue(), m_foodScore);
	SetupValue(m_restValue, "rest", (float)GetRestValue(), m_restScore);
	SetupValue(m_treeValue, "chopTree", (float)GetNumberOfLogs(), m_treeScore);
	SetupValue(m_houseValue, "buildHouse", (float)GetHouseWeight(), m_houseScore);
}

UtilityNPC::~UtilityNPC()
{
	delete m_waterScore;
	delete m_foodScore;
	delete m_restScore;
	delete m_treeScore;
	delete m_houseScore;
}

void UtilityNPC::SelectAction(float deltaTime)
{
	m_waterValue.SetValue((float)GetWaterValue());
	m_foodValue.SetValue((float)GetFoodValue());
	m_restValue.SetValue((float)GetRestValue());
	m_treeValue.SetValue((float)GetNumberOfLogs());
	m_houseValue.SetValue((float)GetHouseWeight());

	float bestScore = 0.f;
	std::string bestAction;
	for (auto score : m_utilityScoreMap)
	{
		float thisScore = score.second->GetUtilityScore();
		if (thisScore > bestScore)
		{
			bestScore = thisScore;
			bestAction = score.first;
		}
	}

	if (bestAction == "collectWater")
		CollectWater(deltaTime);
	if (bestAction == "collectFood")
		CollectFood(deltaTime);
	if (bestAction == "rest")
		Rest(deltaTime);
	if (bestAction == "chopTree")
		ChopTree(deltaTime);
	if (bestAction == "buildHouse")
		BuildHouse(deltaTime);
}

void UtilityNPC::SetupValue(UtilityValue& utilityValue, std::string behaviourName, float value, UtilityScore* utilityScore)
{
	utilityValue.SetNormalizationType(UtilityValue::INVERSE_LINEAR);
	utilityValue.SetMinMaxValues(0, 50);
	utilityValue.SetValue(value);
	utilityScore->AddUtilityValue(&utilityValue, 0.2f);
	m_utilityScoreMap[behaviourName] = utilityScore;
}