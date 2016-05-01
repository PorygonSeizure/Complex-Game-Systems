#include "BaseNPC.h"

#include <iostream>

#include "Gizmos.h"
#include "World.h"

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

using std::cout;
using std::endl;
using glm::vec3;

BaseNPC::BaseNPC(World* world)
{
	m_world = world;
	m_position = vec3(0, 0, 0);

	m_moveSpeed = 5.f;

	m_alive = true;

	m_lastReportTime = 0.f;
	m_reportTime = 1.f;

	m_food = 30;
	m_water = 30;
	m_rested = 30;

	m_numberOfLogs = 0;

	m_houseWeight = 20;

	m_lastFoodReductionTime = 0.f;
	m_lastWaterReductionTime = 0.f;
	m_lastRestedReductionTime = 0.f;

	m_foodReductionTime = 1.f;
	m_waterReductionTime = 1.5f;
	m_restedReductionTime = 2.f;
}

void BaseNPC::Update(float deltaTime)
{
	if (m_alive)
	{
		SelectAction(deltaTime);
	
		CalculateStatusChange();
		ReportStatus();

		CheckAlive();
	}
}

void BaseNPC::Render()
{
	Gizmos::AddAABBFilled(m_position + vec3(0, 2, 0), vec3(0.5, 2, 0.5), glm::vec4(0.4, 0.4, 0.4, 1));
}

void BaseNPC::CollectWater(float deltaTime)
{
	if (TravelTo(m_world->GetWaterLocation(), deltaTime))
	{
		if (m_world->InteractWithWater())
		{
			m_water += 20;
			cout << "Collected Water!" << endl;
		}
	}
}

void BaseNPC::CollectFood(float deltaTime)
{
	if (TravelTo(m_world->GetFoodLocation(), deltaTime))
	{
		if (m_world->InteractWithFood())
		{
			m_food += 20;
			cout << "Collected Food!" << endl;
		}
	}
}

void BaseNPC::Rest(float deltaTime)
{
	if (TravelTo(m_world->GetRestedLocation(), deltaTime))
	{
		if (m_world->InteractWithRested())
		{
			m_rested += 20;
			cout << "Resting!" << endl;
		}
	}
}

void BaseNPC::ChopTree(float deltaTime)
{
	if (TravelTo(m_world->GetTreeLocation(), deltaTime))
	{
		if (m_world->InteractWithTree())
		{
			m_numberOfLogs++;
			cout << "Collected Log!" << endl;
		}
	}
}

void BaseNPC::BuildHouse(float deltaTime)
{
	if (TravelTo(m_world->GetHouseLocation(), deltaTime))
	{
		if (m_numberOfLogs <= 0)
			cout << "Don't have any logs to build with :(" << endl;
		else
		{
			if (m_world->InteractWithHouse())
			{
				m_numberOfLogs--;
				cout << "Built House!" << endl;
				m_world->AddLogToHouse();
			}
		}
	}
}

bool BaseNPC::TravelTo(vec3 loc, float deltaTime)
{
	vec3 dirTo = (loc - m_position);
	float distToPosition = glm::length(dirTo);
	dirTo = glm::normalize(dirTo);

	if (distToPosition <= 0.5)
		return true;
	else
	{
		float moveDist = deltaTime * m_moveSpeed;
		if (distToPosition < moveDist)
			moveDist = distToPosition;

		m_position += dirTo * moveDist;
		return false;
	}
	
}

void BaseNPC::CheckAlive()
{
	if (m_food == 0)
	{
		cout << "Agent has starved to death!" << endl;
		m_alive = false;
	}

	if (m_water == 0)
	{
		cout << "Agent has dehydrated!" << endl;
		m_alive = false;
	}

	if (m_rested == 0)
	{
		cout << "Agent has died due to exhaustion!!" << endl;
		m_alive = false;
	}
}

void BaseNPC::ReportStatus()
{
	float currentTime = (float)glfwGetTime();
	if (currentTime >= m_lastReportTime + m_reportTime)
	{
		m_lastReportTime = currentTime;
		cout << "{ Food: " << m_food;
		cout << ", Water: " << m_water;
		cout << ", Rest: " << m_rested;
		cout << ", Logs: " << m_numberOfLogs;
		cout << " }" << endl;
	}
}

void BaseNPC::CalculateStatusChange()
{
	float currentTime = (float)glfwGetTime();
	if (currentTime >= m_lastFoodReductionTime + m_foodReductionTime)
	{
		m_lastFoodReductionTime = currentTime;
		m_food--;
	}
	if (currentTime >= m_lastWaterReductionTime + m_waterReductionTime)
	{
		m_lastWaterReductionTime = currentTime;
		m_water--;
	}
	if (currentTime >= m_lastRestedReductionTime + m_restedReductionTime)
	{
		m_lastRestedReductionTime = currentTime;
		m_rested--;
	}
}

