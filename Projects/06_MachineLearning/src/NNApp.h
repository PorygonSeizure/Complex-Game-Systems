#ifndef _NN_APP_H_
#define _NN_APP_H_

#include "BaseApp.h"

#include <glm/glm.hpp>

class Agent;
class Turret;
class Food;
class Water;
class LinearZone;

class NNApp : public BaseApp
{
public:
	NNApp();
	virtual ~NNApp() {};

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update(float deltaTime);
	virtual void Draw();

private:
	void SetUpSimulation();
	void SetUpAgents();
	void SetUpTurrets();
	void SetUpFood();
	void SetUpWater();
	void SetUpSimpleLinearZone();
	void UpdateAgents(float delta);
	void DrawAgents();
	void AddAgentGizmos();
	void AddTurretWidgets();
	void AddFoodWidgets();
	void AddWaterWidgets();
	void CheckAgentDamage(Agent* agent);
	void CheckAgentFood(Agent* agent);
	void CheckAgentWater(Agent* agent);

	float SimulateTurret(glm::vec2& centre, float range, Agent* agent);

	static const int MAX_AGENTS = 1;
	static const int MAX_TURRETS = 4;
	static const int MAX_FOOD = 4;
	static const int MAX_WATER = 4;

	//Agent m_agents[MAX_AGENTS];
	//Turret m_turrets[MAX_TURRETS];
	//Food m_foods[MAX_FOOD];
	//Water m_waters[MAX_WATER];
	//LinearZone m_linearZone;

	Agent* m_agents;
	Turret* m_turrets;
	Food* m_foods;
	Water* m_waters;
	LinearZone* m_linearZone;
};

#endif