#ifndef _AGENT_H_
#define _AGENT_H_

#include "glm/glm.hpp"
#include <vector>
#include <Timer.h>

#include "Memory.h"

class Food;
class Water;
class Enemy;

class Agent
{
public:
	Agent();
	virtual ~Agent() {}

	void Setup(glm::vec2 startPos, float size, glm::vec4 colour, float facingDirection);
	void Update(float delta, Food* food, Water* water, Enemy* enemies, int enemiesSize);
	void Draw() {};
	void AddGizmo();

	Memory FoodDeath();
	Memory WaterDeath();
	Memory EnemyContact();
	void FoodContact();
	void WaterContact();

	glm::vec2 GetPosition() { return m_position; }
	glm::vec3 GetWeights() { return glm::vec3(m_foodWeight, m_waterWeight, m_enemyWeight); }
	int GetGen() { return m_generation; }
	//float GetTime() { return m_clock; }
	Memory* GetBestGens() { return m_bestGen; }

	float m_baseFoodWeight;
	float m_baseWaterWeight;
	float m_baseEnemyWeight;
	float m_hunger;
	float m_thrist;

	std::vector<float> foodValues;
	std::vector<float> waterValues;
	std::vector<float> enemyValues;
	std::vector<float> times;

private:
	//float m_facingDirection;
	float m_diameter;
	Timer m_clock;
	//float m_clock;
	float m_maxSpeed;

	glm::vec2 m_position;
	glm::vec2 m_startingPosition;
	glm::vec2 m_velocity;

	bool m_active;

	glm::vec4 m_colour;

	std::vector<Memory> m_memory;
	
	bool CheckBounds();

	int m_generation;

	Memory AddToMemory(Death death);

	void Behaviour(float deltaTime, Food* food, Water* water, Enemy* enemies, int enemiesSize);
	void ModifyNewGen();

	float m_foodWeight;
	float m_waterWeight;
	float m_enemyWeight;

	Memory m_bestGen[3];

	bool m_firstContact;

	float m_foodDeath;
	float m_waterDeath;
	float m_enemyDeath;

	Food* m_food;
	Water* m_water;

	int m_betweenGoodGens;
};

#endif