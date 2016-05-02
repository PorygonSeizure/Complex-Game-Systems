#include "Agent.h"
#include <GLFW/glfw3.h>
#include "Gizmos.h"
#include "Food.h"
#include "Water.h"

#include <iostream>
#include "Enemy.h"

using glm::vec3;
using glm::vec2;

Agent::Agent()
{
	m_baseFoodWeight = 150.f;
	m_baseWaterWeight = 150.f;
	m_baseEnemyWeight = 000.f;
	m_generation = 1;
	m_foodDeath = 0;
	m_waterDeath = 0;
	m_enemyDeath = 0;

	for (int i = 0; i < 3; i++)
	{
		m_bestGen[i].food = 150.f;
		m_bestGen[i].water = 150.f;
		m_bestGen[i].enemy = 000.f;
		m_bestGen[i].time = 0.f;
	}
}

void Agent::Update(float delta, Food* food, Water* water, Enemy* enemy, int enemiesSize)
{
	//m_clock += delta;
	m_food = food;
	m_water = water;
	Behaviour(delta, food, water, enemy, enemiesSize);
	m_position += m_velocity * delta;
	if (CheckBounds())
	{
		//m_facingDirection = 44.f / 7.f * (float)((rand() % 1000) / 1000.f);
		if (m_velocity.x > m_maxSpeed)
			m_velocity.x = m_maxSpeed;
		if (m_velocity.x < -m_maxSpeed)
			m_velocity.x = -m_maxSpeed;
		if (m_velocity.y > m_maxSpeed)
			m_velocity.y = m_maxSpeed;
		if (m_velocity.y < -m_maxSpeed)
			m_velocity.y = -m_maxSpeed;
	}
	m_hunger--;
	m_thrist--;
}

bool Agent::CheckBounds()
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &screenWidth, &screenHeight);

	if (m_position.x < 0 && m_velocity.x < 0)
	{
		m_velocity.x = -m_velocity.x;
		return true;
	}
	if (m_position.y < 0 && m_velocity.y < 0)
	{
		m_velocity.y = -m_velocity.y;
		return true;
	}
	if (m_position.x > screenWidth  && m_velocity.x > 0)
	{
		m_velocity.x = -m_velocity.x;
		return true;
	}
	if (m_position.y > screenHeight  && m_velocity.y > 0)
	{
		m_velocity.y = -m_velocity.y;
		return true;
	}
	return false;
}

void Agent::Setup(vec2 startPos, float size, glm::vec4 colour, float facingDirection)
{
	if (m_memory.size() >= 10)
		ModifyNewGen();
	m_position = startPos;
	m_startingPosition = startPos;
	//m_facingDirection = facingDirection;
	m_diameter = size;
	//m_clock = 0.f;
	m_colour = colour;
	m_maxSpeed = 500.f;
	m_velocity.x = m_maxSpeed / 4.f;// * sin(m_facingDirection);
	m_velocity.y = m_maxSpeed / 4.f;// * cos(m_facingDirection);
	m_hunger = 500.f;
	m_thrist = 500.f;
	m_foodWeight = rand() % 10 + (m_baseFoodWeight - 5);
	m_waterWeight = rand() % 10 + (m_baseWaterWeight - 5);
	m_enemyWeight = rand() % 10 + (m_baseEnemyWeight - 5);

	m_clock.Start();
	m_firstContact = false;
}

Memory Agent::AddToMemory(Death death)
{
	m_clock.End();
	Memory newMemory;
	newMemory.generation = m_generation;
	newMemory.food = m_foodWeight;
	newMemory.water = m_waterWeight;
	newMemory.enemy = m_enemyWeight;
	newMemory.time = (float)m_clock.GetTimeDiff();
	//newMemory.time = m_clock;
	newMemory.death = death;
	m_memory.push_back(newMemory);
	std::cout << newMemory.time << std::endl;
	return newMemory;
}

void Agent::AddGizmo()
{
	int segments = 20;
	Gizmos::Add2DCircle(m_position, m_diameter * 1.1f / 2.f, segments, glm::vec4(0, 0, 0, 1));
	Gizmos::Add2DCircle(m_position, m_diameter * 0.9f / 2.f, segments, m_colour);
}

Memory Agent::FoodDeath()
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &screenWidth, &screenHeight);
	Memory newMemory;
	if (m_firstContact)
		newMemory = AddToMemory(STARVED);
	else
	{
		if (glm::distance(m_position, m_food->GetPos()) < glm::distance(m_position, m_water->GetPos()))
			newMemory = AddToMemory(NOCONTACTCLOSERTOFOOD);
		else if (glm::distance(m_position, m_food->GetPos()) > glm::distance(m_position, m_water->GetPos()))
			newMemory = AddToMemory(NOCONTACTCLOSERTOWATER);
	}
	m_startingPosition.x = 600.f;//(float)(rand() % screenWidth);
	m_startingPosition.y = 100.f;//(float)(rand() % 2 * screenHeight);
	float size = 20.f;
	float facing = 0.f;//44.f / 7.f * (float)((rand() % 1000) / 1000.f);
	Setup(m_startingPosition, m_diameter, m_colour, facing);
	return newMemory;
}

Memory Agent::WaterDeath()
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &screenWidth, &screenHeight);
	Memory newMemory;
	if (m_firstContact)
		newMemory = AddToMemory(DEHYDRATED);
	else if (glm::distance(m_position, m_food->GetPos()) < glm::distance(m_position, m_water->GetPos()))
		newMemory = AddToMemory(NOCONTACTCLOSERTOFOOD);
	else if (glm::distance(m_position, m_food->GetPos()) > glm::distance(m_position, m_water->GetPos()))
		newMemory = AddToMemory(NOCONTACTCLOSERTOWATER);
	m_startingPosition.x = 600.f;//(float)(rand() % screenWidth);
	m_startingPosition.y = 100.f;//(float)(rand() % 2 * screenHeight);
	float size = 20.f;
	float facing = 0.f;//44.f / 7.f * (float)((rand() % 1000) / 1000.f);
	Setup(m_startingPosition, m_diameter, m_colour, facing);
	return newMemory;
}

Memory Agent::EnemyContact()
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &screenWidth, &screenHeight);
	Memory newMemory;
	if (m_firstContact)
		newMemory = AddToMemory(KILLED);
	else if (glm::distance(m_position, m_food->GetPos()) < glm::distance(m_position, m_water->GetPos()))
		newMemory = AddToMemory(KILLEDCLOSERTOFOOD);
	else if (glm::distance(m_position, m_food->GetPos()) > glm::distance(m_position, m_water->GetPos()))
		newMemory = AddToMemory(KILLEDCLOSERTOWATER);
	m_startingPosition.x = 600.f;//(float)(rand() % screenWidth);
	m_startingPosition.y = 100.f;//(float)(rand() % 2 * screenHeight);
	float size = 20.f;
	float facing = 0.f;//44.f / 7.f * (float)((rand() % 1000) / 1000.f);
	Setup(m_startingPosition, m_diameter, m_colour, facing);
	return newMemory;
}

void Agent::FoodContact()
{
	m_hunger += 100;
	m_firstContact = true;
}

void Agent::WaterContact()
{
	m_thrist += 100;
	m_firstContact = true;
}

void Agent::Behaviour(float deltaTime, Food* food, Water* water, Enemy* enemies, int enemiesSize)
{
	vec2 temp1 = glm::normalize(food->GetPos() - m_position) * m_maxSpeed;
	vec2 force1 = (temp1 - m_velocity) * m_foodWeight / 300.f;
	vec2 temp2 = glm::normalize(water->GetPos() - m_position) * m_maxSpeed;
	vec2 force2 = (temp2 - m_velocity) * m_waterWeight / 300.f;
	vec2 temp3;
	vec2 force3;
	for (int i = 0; i < enemiesSize; i++)
	{
		temp3 = glm::normalize(m_position - enemies[i].GetPos()) * m_maxSpeed;
		force3 += (temp3 - m_velocity) * (m_enemyWeight / (float)enemiesSize);
	}

	force3 /= 300.f;

	vec2 totalForce = force1 + force2 + force3;
	if (totalForce.x >= m_maxSpeed)
		totalForce.x = m_maxSpeed;
	if (totalForce.y >= m_maxSpeed)
		totalForce.y = m_maxSpeed;
	if (totalForce.x <= -m_maxSpeed)
		totalForce.x = -m_maxSpeed;
	if (totalForce.y <= -m_maxSpeed)
		totalForce.y = -m_maxSpeed;
	m_velocity += totalForce * deltaTime;
}

void Agent::ModifyNewGen()
{
	//Memory parent1;
	//Memory parent2;
	//parent1.time = 0.f;
	//parent2.time = (float)INT_MAX;

	int i = 0;
	if (m_generation % 20 == 0)
		i = 5;

	float timeTotal = 0.f;

	for (auto e : m_memory)
	{
		timeTotal += e.time;

		//if (parent1.time <= e.time)
		//{
		//	parent1 = e;
		//}
		//else if (parent2.time >= e.time)
		//{
		//	parent2 = e;
		//}
	}

	timeTotal /= 10.f;

	if (timeTotal >= m_bestGen[0].time)
	{
		m_bestGen[2] = m_bestGen[1];
		m_bestGen[1] = m_bestGen[0];
		m_bestGen[0].generation = m_generation;
		m_bestGen[0].food = m_baseFoodWeight;
		m_bestGen[0].water = m_baseWaterWeight;
		m_bestGen[0].enemy = m_baseEnemyWeight;
		m_bestGen[0].time = timeTotal;
	}
	else if (timeTotal >= m_bestGen[1].time)
	{
		m_bestGen[2] = m_bestGen[1];
		m_bestGen[1].generation = m_generation;
		m_bestGen[1].food = m_baseFoodWeight;
		m_bestGen[1].water = m_baseWaterWeight;
		m_bestGen[1].enemy = m_baseEnemyWeight;
		m_bestGen[1].time = timeTotal;
	}
	else if (timeTotal >= m_bestGen[2].time)
	{
		m_bestGen[2].generation = m_generation;
		m_bestGen[2].food = m_baseFoodWeight;
		m_bestGen[2].water = m_baseWaterWeight;
		m_bestGen[2].enemy = m_baseEnemyWeight;
		m_bestGen[2].time = timeTotal;
	}

	m_generation++;

	//float baseTime = 8.346f;

	for (auto e : m_memory)
	{
		switch (e.death)
		{
		case STARVED:
			//m_baseFoodWeight += 2.f;
			//m_baseWaterWeight--;
			//m_baseEnemyWeight--;
			m_foodDeath += (e.time / timeTotal);
			break;
		case DEHYDRATED:
			//m_baseFoodWeight--;
			//m_baseWaterWeight += 2.f;
			//m_baseEnemyWeight--;
			m_waterDeath += (e.time / timeTotal);
			break;
		case KILLED:
			//m_baseFoodWeight--;
			//m_baseWaterWeight--;
			//m_baseEnemyWeight += 2.f;
			m_enemyDeath += (e.time / timeTotal);
			break;
		case NOCONTACTCLOSERTOFOOD:
			m_foodDeath += (2.f * (e.time / timeTotal));
			//m_foodDeath -= (e.time / timeTotal);
			m_enemyDeath -= (e.time / timeTotal);
			break;
		case NOCONTACTCLOSERTOWATER:
			m_waterDeath += (2.f * (e.time / timeTotal));
			//m_waterDeath -= (e.time / timeTotal);
			m_enemyDeath -= (e.time / timeTotal);
			break;
		case KILLEDCLOSERTOFOOD:
			m_enemyDeath += (2.f * (e.time / timeTotal));
			m_foodDeath -= (e.time / timeTotal);
			break;
		case KILLEDCLOSERTOWATER:
			m_enemyDeath += (2.f * (e.time / timeTotal));
			m_waterDeath -= (e.time / timeTotal);
			break;
		case NOCONTACT:
			//m_baseFoodWeight++;
			//m_baseWaterWeight++;
			//m_baseEnemyWeight -= 2.f;
			//m_foodDeath++;
			//m_waterDeath++;
			//m_enemyDeath--;
			break;
		}
	}

	float total = m_foodDeath + m_waterDeath + m_enemyDeath;

	//if (m_foodDeath > (total / 2.f) || m_waterDeath > (total / 2.f) || m_enemyDeath > (total / 2.f))
	//{
	//	m_baseFoodWeight = (m_bestGen[0].food + m_bestGen[1].food + m_bestGen[2].food) / 3.f;
	//	m_baseWaterWeight = (m_bestGen[0].water + m_bestGen[1].water + m_bestGen[2].water) / 3.f;
	//	m_baseEnemyWeight = (m_bestGen[0].enemy + m_bestGen[1].enemy + m_bestGen[2].enemy) / 3.f;
	//}

	//if (m_foodDeath > (total / 2.f))
	//{
	//	m_baseFoodWeight += 10.f;
	//	m_baseWaterWeight -= 5.f;
	//	m_baseEnemyWeight -= 5.f;
	//}
	//
	//if (m_waterDeath > (total / 2.f))
	//{
	//	m_baseWaterWeight += 10.f;
	//	m_baseFoodWeight -= 5.f;
	//	m_baseEnemyWeight -= 5.f;
	//}
	//
	//if (m_enemyDeath > (total / 2.f))
	//{
	//	m_baseEnemyWeight += 10.f;
	//	m_baseFoodWeight -= 5.f;
	//	m_baseWaterWeight -= 5.f;
	//}
	//
	//if (m_foodDeath <= 0.f)
	//{
	//	m_baseFoodWeight -= 10.f;
	//	m_baseWaterWeight += 5.f;
	//	m_baseEnemyWeight += 5.f;
	//}
	//
	//if (m_waterDeath <= 0.f)
	//{
	//	m_baseWaterWeight -= 10.f;
	//	m_baseFoodWeight += 5.f;
	//	m_baseEnemyWeight += 5.f;
	//}
	//
	//if (m_enemyDeath <= 0.f)
	//{
	//	m_baseEnemyWeight -= 10.f;
	//	m_baseFoodWeight += 5.f;
	//	m_baseWaterWeight += 5.f;
	//}

	m_baseFoodWeight -= (total / 3.f);
	m_baseWaterWeight -= (total / 3.f);
	m_baseEnemyWeight -= (total / 3.f);
	m_baseFoodWeight += m_foodDeath;
	m_baseWaterWeight += m_waterDeath;
	m_baseEnemyWeight += m_enemyDeath;

	m_foodDeath = 0;
	m_waterDeath = 0;
	m_enemyDeath = 0;

	m_memory.clear();
}