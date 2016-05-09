#ifndef _GENETIC_APP_H_
#define _GENETIC_APP_H_

#include "BaseApp.h"

#include <glm/glm.hpp>
#include <vector>

#include "Memory.h"

class Agent;
class Enemy;
class Food;
class Water;
class LinearZone;

class GeneticApp : public BaseApp
{
public:
	GeneticApp();
	virtual ~GeneticApp() {};
	
	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update(float deltaTime);
	virtual void Draw();

private:
	void SetUpSimulation();
	void SetUpAgents();
	void SetUpEnemy();
	void SetUpFood();
	void SetUpWater();
	void UpdateAgents(float delta);
	void DrawAgents();
	void AddAgentGizmos();
	void AddEnemyWidgets(float delta);
	void AddFoodWidgets();
	void AddWaterWidgets();
	void CheckAgentDamage(Agent* agent);
	void CheckAgentFood(Agent* agent);
	void CheckAgentWater(Agent* agent);

	float SimulateEnemy(glm::vec2& centre, float range, Agent* agent);

	//static const int MAX_AGENTS = 1;
	static const int MAX_ENEMY = 2;
	//static const int MAX_FOOD = 4;
	//static const int MAX_WATER = 4;

	Agent* m_agent;
	Enemy* m_enemies;
	Food* m_food;
	Water* m_water;

	std::vector<Memory> m_memory;
};

#endif