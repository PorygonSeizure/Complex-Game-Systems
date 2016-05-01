#include "NNApp.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "Gizmos.h"

#include "Agent.h"
#include "Turret.h"
#include "Food.h"
#include "Water.h"
#include "LinearZone.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::vec2;

NNApp::NNApp()
{
	m_agents = new Agent[MAX_AGENTS];
	m_turrets = new Turret[MAX_TURRETS];
	m_foods = new Food[MAX_FOOD];
	m_waters = new Water[MAX_WATER];
	m_linearZone = new LinearZone();
}

bool NNApp::Startup()
{
	//create a basic window
	CreateGLFWWindow("AIE OpenGL Application", 1280, 720);

	//start the gizmo system that can draw basic shapes
	Gizmos::Create(0xffff, 0xffff, 0xffff, 0xffff);
	
	SetUpSimulation();

	return true;
}

void NNApp::Shutdown()
{
	Gizmos::Destroy();

	//destroy our window properly
	DestroyGLFWWindow();
}

bool NNApp::Update(float deltaTime)
{
	//close the application if the window closes
	if (glfwWindowShouldClose(m_window) || glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;
	
	//clear the gizmos out for this frame
	Gizmos::Clear();

	UpdateAgents(deltaTime);
	//m_linearZone.AddGizmo();
	m_linearZone->AddGizmo();
	AddAgentGizmos();
	AddTurretWidgets();
	AddFoodWidgets();
	AddWaterWidgets();

	//return true, else the application closes
	return true;
}

void NNApp::Draw()
{
	//clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//get a orthographic projection matrix and draw 2D gizmos
	int width = 0;
	int height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	mat4 guiMatrix = glm::ortho<float>(0.f, (float)width, 0.f, (float)height);

	Gizmos::Draw2D(guiMatrix);
}

void NNApp::SetUpSimulation()
{
	SetUpAgents();
	SetUpSimpleLinearZone();
	SetUpTurrets();
	SetUpFood();
	SetUpWater();
}

void NNApp::SetUpSimpleLinearZone()
{
	vec2 centre(600, 400);
	float rotation = 1.5f;
	//m_linearZone = LinearZone(centre, rotation);
	m_linearZone = new LinearZone(centre, rotation);
}

void NNApp::SetUpAgents()
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(m_window, &screenWidth, &screenHeight);

	srand((unsigned int)time(NULL));
	for (int index = 0; index < MAX_AGENTS; index++)
	{
		vec2 startPos;
		startPos.x = (float)(rand() % screenWidth);
		startPos.y = (float)(rand() % 2 * screenHeight);
		float size = 30.f;
		float facing = 44.f / 7.f * (float)((rand() % 1000) / 1000.f);
		m_agents[index].Setup(startPos, size, vec4(1, 1, 1, 1), facing);
	}
}

void NNApp::UpdateAgents(float delta)
{
	for (int index = 0; index < MAX_AGENTS; index++)
	{
		m_agents[index].Update(delta);
		CheckAgentDamage(&m_agents[index]);
		CheckAgentFood(&m_agents[index]);
		CheckAgentWater(&m_agents[index]);
	}
}

void NNApp::CheckAgentDamage(Agent* agent)
{
	float damage = 0.f;
	//damage += m_linearZone.CheckRange(agent->GetPosition());
	damage += m_linearZone->CheckRange(agent->GetPosition());
	for (int i = 0; i < MAX_TURRETS; i++)
	{
		damage += m_turrets[i].CheckRange(agent->GetPosition());
	}
	agent->HurtAgent(damage);
}

void NNApp::CheckAgentFood(Agent* agent)
{
	float foodFound = 0.f;
	for (int i = 0; i < MAX_FOOD; i++)
	{
		foodFound += m_foods[i].CheckRange(agent->GetPosition());
	}
	agent->FeedAgent(foodFound);
}

void NNApp::CheckAgentWater(Agent* agent)
{
	float waterFound = 0.f;
	for (int i = 0; i < MAX_WATER; i++)
	{
		waterFound += m_waters[i].CheckRange(agent->GetPosition());
	}
	agent->HydrateAgent(waterFound);
}

void NNApp::SetUpTurrets()
{
	m_turrets[0] = Turret(vec2(400, 300), 200.f);
	//m_turrets[1] = Turret(vec2(1100, 300), 200.f); //second turret for next experiment
}

void NNApp::SetUpFood()
{
	m_foods[0] = Food(vec2(900, 50), 75.f);
	//m_foods[1] = Food(vec2(600, 600), 75.f);
}

void NNApp::SetUpWater()
{
	//m_waters[0] = Water(vec2(50, 500), 75.f);
	//m_waters[1] = Water(vec2(1000, 600), 75.f);
}

float NNApp::SimulateTurret(vec2& centre, float range, Agent* agent)
{
	vec2 displacment = agent->GetPosition() - centre;
	float distance = glm::length(displacment);
	if (distance < range)
	{
		return true;
	}
	return false;
}

void NNApp::DrawAgents()
{
	for (int i = 0; i < MAX_AGENTS; i++)
	{
		m_agents[i].Draw();
	}
}

void NNApp::AddAgentGizmos()
{
	for (int i = 0; i < MAX_AGENTS; i++)
	{
		m_agents[i].AddGizmo();
	}
}

void NNApp::AddTurretWidgets()
{
	for (int i = 0; i < MAX_TURRETS; i++)
	{
		m_turrets[i].AddGizmo();
	}
}

void NNApp::AddFoodWidgets()
{
	for (int i = 0; i < MAX_FOOD; i++)
	{
		m_foods[i].AddGizmo();
	}
}

void NNApp::AddWaterWidgets()
{
	for (int i = 0; i < MAX_WATER; i++)
	{
		m_waters[i].AddGizmo();
	}
}