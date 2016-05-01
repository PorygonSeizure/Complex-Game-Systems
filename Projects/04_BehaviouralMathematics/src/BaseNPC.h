#ifndef _BASE_NPC_H_
#define _BASE_NPC_H_

#include <glm/glm.hpp>

class World;

class BaseNPC
{
public:
	//Don't want any base constructor
	BaseNPC() = delete;

	BaseNPC(World* world);
	virtual ~BaseNPC() {};

	void Update(float deltaTime);
	void Render();

	//This will just output the NPC's vitals and information to the console
	void ReportStatus();

	unsigned int GetWaterValue() const { return m_water; }
	unsigned int GetFoodValue() const { return m_food; }
	unsigned int GetRestValue() const { return m_rested; }
	unsigned int GetNumberOfLogs() const { return m_numberOfLogs; }
	unsigned int GetHouseWeight() const { return m_houseWeight; }

	glm::vec3 GetPosition() const { return m_position; }
protected:
	//Called every frame by update - should call one of the behaviour functions below.
	virtual void SelectAction(float deltaTime) { CollectWater(deltaTime); }

	//Different behaviours that our AI can run - these will move the AI towards the required location
	//and then do the task.
	void CollectWater(float deltaTime);
	void CollectFood(float deltaTime);
	void Rest(float deltaTime);
	
	void ChopTree(float deltaTime);
	void BuildHouse(float deltaTime);

	World* m_world;
private:
	bool TravelTo(glm::vec3 loc, float deltaTime);
	
	void CheckAlive();
	void CalculateStatusChange();
	glm::vec3 m_position;

	unsigned int m_food;
	unsigned int m_water;
	unsigned int m_rested;

	unsigned int m_numberOfLogs;

	unsigned int m_houseWeight;

	float m_moveSpeed;

	bool m_alive;

	float m_lastReportTime;
	float m_reportTime;

	float m_lastFoodReductionTime;
	float m_lastWaterReductionTime;
	float m_lastRestedReductionTime;

	float m_foodReductionTime;
	float m_waterReductionTime;
	float m_restedReductionTime;
};

#endif