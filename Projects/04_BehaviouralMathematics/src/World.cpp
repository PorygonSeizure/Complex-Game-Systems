#include "World.h"
#include "Gizmos.h"

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

#include <iostream>

using glm::vec3;
using glm::vec4;

World::World()
{
	m_houseLocation = vec3(10, 0, -10);
	m_treeLocation = vec3(-10, 0, -10);

	m_foodLocation = vec3(10, 0, 10);
	m_waterLocation = vec3(0, 0, 10);
	m_restedLocation = vec3(-10, 0, 10);

	m_houseCurrentLogs = 4;
	m_houseLogsRequired = 10;

	m_restedInteractTime = 1.f;
	m_waterInteractTime = 2.f;
	m_foodInteractTime =  2.f;

	m_houseInteractTime = 3.0;
	m_treeInteractTime = 1.f;

	m_lastFoodInteractTime = 0.f;
	m_lastWaterInteractTime = 0.f;
	m_lastRestedInteractTime = 0.f;
	m_lastHouseInteractTime = 0.f;
	m_lastTreeInteractTime = 0.f;

}

void World::Render()
{
	Gizmos::AddSphere(m_foodLocation, 1, 8, 8, vec4(1, 0, 0, 1));
	Gizmos::AddSphere(m_waterLocation, 1, 8, 8, vec4(0, 0, 1, 1));
	Gizmos::AddSphere(m_restedLocation, 1, 8, 8, vec4(0, 1, 1, 1));

	float m_houseHeight = 3 * ((float)m_houseCurrentLogs / m_houseLogsRequired);
	vec4 houseColor = (m_houseCurrentLogs >= m_houseLogsRequired) ? vec4(0, 1, 0, 1) : vec4(1, 1, 0, 1);

	Gizmos::AddAABBFilled(m_houseLocation + vec3(0, m_houseHeight, 0), vec3(3, m_houseHeight, 2), houseColor);

	Gizmos::AddCylinderFilled(m_treeLocation, 1, 2, 8, vec4(0, 1, 0, 1));
}

void World::AddLogToHouse()
{
	if (m_houseCurrentLogs < m_houseLogsRequired)
		m_houseCurrentLogs++;
}

bool World::InteractWithFood()
{
	float currentTime = (float)glfwGetTime();
	if (currentTime >= m_lastFoodInteractTime + m_foodInteractTime)
	{
		m_lastFoodInteractTime = currentTime;
		return true;
	}
	return false;
}

bool World::InteractWithWater()
{
	float currentTime = (float)glfwGetTime();

	if (currentTime >= m_lastWaterInteractTime + m_waterInteractTime)
	{
		m_lastWaterInteractTime = currentTime;
		return true;
	}

	return false;
}

bool World::InteractWithRested()
{
	float currentTime = (float)glfwGetTime();
	if (currentTime >= m_lastRestedInteractTime + m_restedInteractTime)
	{
		m_lastRestedInteractTime = currentTime;
		return true;
	}
	return false;
}

bool World::InteractWithTree()
{
	float currentTime = (float)glfwGetTime();
	if (currentTime >= m_lastTreeInteractTime + m_treeInteractTime)
	{
		m_lastTreeInteractTime = currentTime;
		return true;
	}
	return false;
}

bool World::InteractWithHouse()
{
	float currentTime = (float)glfwGetTime();
	if (currentTime >= m_lastHouseInteractTime + m_houseInteractTime)
	{
		m_lastHouseInteractTime = currentTime;
		return true;
	}
	return false;
}
