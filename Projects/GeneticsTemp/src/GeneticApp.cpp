#include "GeneticApp.h"
#include "gl_core_4_4.h"
#include <imgui.h>
#include "ImGui/imgui_impl_glfw_gl3.h"

#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include <stdlib.h>

#include "Gizmos.h"

#include "Agent.h"
#include "Enemy.h"
#include "Food.h"
#include "Water.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::vec2;

GeneticApp::GeneticApp()
{
	//m_agents = new Agent[MAX_AGENTS];
	m_enemies = new Enemy[MAX_ENEMY];
	//m_foods = new Food[MAX_FOOD];
	//m_waters = new Water[MAX_WATER];

	m_agent = new Agent();
	//m_enemy = new Enemy();
	m_food = new Food();
	m_water = new Water();
}

bool GeneticApp::Startup()
{
	//create a basic window
	CreateGLFWWindow("AIE OpenGL Application", 1280, 720);

	//start the gizmo system that can draw basic shapes
	Gizmos::Create(0xffff, 0xffff, 0xffff, 0xffff);

	ImGui_ImplGlfwGL3_Init(m_window, true);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = 1280.f;
	io.DisplaySize.y = 720.f;
	
	SetUpSimulation();

	return true;
}

void GeneticApp::Shutdown()
{
	delete m_agent;
	delete m_food;
	delete m_water;
	delete[] m_enemies;

	Gizmos::Destroy();

	//destroy our window properly
	DestroyGLFWWindow();
}

bool GeneticApp::Update(float deltaTime)
{
	//close the application if the window closes
	if (glfwWindowShouldClose(m_window) || glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;
	
	//clear the gizmos out for this frame
	Gizmos::Clear();

	UpdateAgents(deltaTime);
	AddAgentGizmos();
	AddEnemyWidgets(deltaTime);
	AddFoodWidgets();
	AddWaterWidgets();

	//return true, else the application closes
	return true;
}

void GeneticApp::Draw()
{
	//clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplGlfwGL3_NewFrame();

	//get a orthographic projection matrix and draw 2D gizmos
	int width = 0;
	int height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	mat4 guiMatrix = glm::ortho<float>(0.f, (float)width, 0.f, (float)height);

	Gizmos::Draw2D(guiMatrix);

	vec3 weights = m_agent->GetWeights();
	int gen = m_agent->GetGen();
	//float time = m_agent->GetTime();

	if (ImGui::Begin("My rendering options"))
	{
		ImGui::SliderFloat("Base Food Weight", &m_agent->m_baseFoodWeight, -300.f, 300.f);
		ImGui::SliderFloat("Base Water Weight", &m_agent->m_baseWaterWeight, -300.f, 300.f);
		ImGui::SliderFloat("Base Enemy Weight", &m_agent->m_baseEnemyWeight, -300.f, 300.f);
		ImGui::SliderFloat("Hunger Level", &m_agent->m_hunger, 0.f, 2000.f);
		ImGui::SliderFloat("Thrist Level", &m_agent->m_thrist, 0.f, 2000.f);
		if (ImGui::CollapsingHeader("Current Agent"))
		{
			ImGui::Text("Food weight %f", weights.x);
			ImGui::Text("Water weight %f", weights.y);
			ImGui::Text("Enemy weight %f", weights.z);
			ImGui::Text("Generation %d", gen);
		}
		char generation[32];
		for (int i = 1; i < gen + 1; i++)
		{
			snprintf(generation, 32, "%d", i);
			if (ImGui::CollapsingHeader(generation))
			{
				ImGui::BeginChild("Agent Memory", ImVec2(0, 250), true);
				ImGui::Columns(6);
				for (auto agent : m_memory)
				{
					if (agent.generation == i)
					{
						char buffer[32];
						snprintf(buffer, 32, "%d", agent.generation);
						ImGui::Button(buffer, ImVec2(-1.f, 0.f));
						ImGui::NextColumn();
						snprintf(buffer, 32, "%f", agent.food);
						ImGui::Button(buffer, ImVec2(-1.f, 0.f));
						ImGui::NextColumn();
						snprintf(buffer, 32, "%f", agent.water);
						ImGui::Button(buffer, ImVec2(-1.f, 0.f));
						ImGui::NextColumn();
						snprintf(buffer, 32, "%f", agent.enemy);
						ImGui::Button(buffer, ImVec2(-1.f, 0.f));
						ImGui::NextColumn();
						snprintf(buffer, 32, "%f", agent.time);
						ImGui::Button(buffer, ImVec2(-1.f, 0.f));
						ImGui::NextColumn();
						if (agent.death == STARVED)
							ImGui::Button("Hunger", ImVec2(-1.f, 0.f));
						else if (agent.death == DEHYDRATED)
							ImGui::Button("Thrist", ImVec2(-1.f, 0.f));
						else if (agent.death == KILLED)
							ImGui::Button("Killed", ImVec2(-1.f, 0.f));
						else if (agent.death == NOCONTACT)
							ImGui::Button("No Contact", ImVec2(-1.f, 0.f));
						else if (agent.death == NOCONTACTCLOSERTOFOOD)
							ImGui::Button("No Contact (Food)", ImVec2(-1.f, 0.f));
						else if (agent.death == NOCONTACTCLOSERTOWATER)
							ImGui::Button("No Contact (Water)", ImVec2(-1.f, 0.f));
						else if (agent.death == KILLEDCLOSERTOFOOD)
							ImGui::Button("Killed (Food)", ImVec2(-1.f, 0.f));
						else if (agent.death == KILLEDCLOSERTOWATER)
							ImGui::Button("Killed (Water)", ImVec2(-1.f, 0.f));
						else if (agent.death == NOCONTACT)
							ImGui::Button("No Contact", ImVec2(-1.f, 0.f));
						ImGui::NextColumn();
					}
				}
				ImGui::EndChild();
			}
		}
	}
	ImGui::End();

	ImGui::Render();
}

void GeneticApp::SetUpSimulation()
{
	SetUpAgents();
	SetUpEnemy();
	SetUpFood();
	SetUpWater();
}

void GeneticApp::SetUpAgents()
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(m_window, &screenWidth, &screenHeight);

	srand((unsigned int)time(NULL));
	//for (int index = 0; index < MAX_AGENTS; index++)
	//{
		vec2 startPos;
		//startPos.x = (float)(rand() % screenWidth);
		//startPos.y = (float)(rand() % 2 * screenHeight);
		//float size = 30.f;
		//float facing = 44.f / 7.f * (float)((rand() % 1000) / 1000.f);
		startPos.x = 600.f;
		startPos.y = 100.f;
		float size = 20.f;
		float facing = 0.f;
		//m_agents[index].Setup(startPos, size, vec4(1, 1, 1, 1), facing);
		m_agent->Setup(startPos, size, vec4(1, 1, 1, 1), facing);
	//}
}

void GeneticApp::UpdateAgents(float delta)
{
	/*for (int index = 0; index < MAX_AGENTS; index++)
	{
		m_agents[index].Update(delta, m_food, m_water, m_enemy);
		CheckAgentDamage(&m_agents[index]);
		CheckAgentFood(&m_agents[index]);
		CheckAgentWater(&m_agents[index]);
	}*/

	m_agent->Update(delta, m_food, m_water, m_enemies, MAX_ENEMY);
	CheckAgentDamage(m_agent);
	CheckAgentFood(m_agent);
	CheckAgentWater(m_agent);
}

void GeneticApp::CheckAgentDamage(Agent* agent)
{
	//float damage = 0.f;
	//for (int i = 0; i < MAX_ENEMY; i++)
	//	damage += m_enemies[i].CheckRange(agent->GetPosition());
	//damage += m_enemy->CheckRange(agent->GetPosition());
	//agent->EnemyContact(damage);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (m_enemies[i].CheckRange(agent->GetPosition()))
		{
			m_memory.push_back(agent->EnemyContact());
			SetUpEnemy();
			return;
		}
	}
}

void GeneticApp::CheckAgentFood(Agent* agent)
{
	//float foodFound = 0.f;
	//for (int i = 0; i < MAX_FOOD; i++)
	//	foodFound += m_foods[i].CheckRange(agent->GetPosition());
	//foodFound += m_food->CheckRange(agent->GetPosition());
	//agent->FoodContact();

	if (m_food->CheckRange(agent->GetPosition()))
	{
		agent->FoodContact();
		SetUpFood();
	}
	if (agent->m_hunger < 1) { m_memory.push_back(agent->FoodDeath()); SetUpEnemy(); }
}

void GeneticApp::CheckAgentWater(Agent* agent)
{
	//float waterFound = 0.f;
	//for (int i = 0; i < MAX_WATER; i++)
	//	waterFound += m_waters[i].CheckRange(agent->GetPosition());
	//waterFound += m_water->CheckRange(agent->GetPosition());
	//agent->WaterContact(waterFound);

	if (m_water->CheckRange(agent->GetPosition()))
	{
		agent->WaterContact();
		SetUpWater();
	}
	if (agent->m_thrist < 1) { m_memory.push_back(agent->WaterDeath()); SetUpEnemy(); }
}

void GeneticApp::SetUpEnemy()
{
	m_enemies[0] = Enemy(vec2(1200, 700), 25.f);
	m_enemies[1] = Enemy(vec2(1100, 600), 25.f);

	//int screenWidth = 0;
	//int screenHeight = 0;
	//glfwGetWindowSize(m_window, &screenWidth, &screenHeight);
	//float facing = 44.f / 7.f * (float)((rand() % 1000) / 1000.f);
	//for (int i = 0; i < MAX_ENEMY; i++)
	//	m_enemies[i] = Enemy(vec2(rand() % screenWidth, rand() % screenHeight), 25.f, facing);
}

void GeneticApp::SetUpFood()
{
	delete m_food;
	m_food = new Food(vec2(100, 600), 75.f);
	//m_foods[0] = Food(vec2(900, 50), 75.f);
	//m_foods[1] = Food(vec2(600, 600), 75.f);

	//int screenWidth = 0;
	//int screenHeight = 0;
	//glfwGetWindowSize(m_window, &screenWidth, &screenHeight);
	//int x = rand() % screenWidth;
	//int y = rand() % screenHeight;
	//delete m_food;
	//m_food = new Food(vec2(x, y), 75.f);
}

void GeneticApp::SetUpWater()
{
	delete m_water;
	m_water = new Water(vec2(900, 600), 75.f);
	//m_waters[0] = Water(vec2(50, 500), 75.f);
	//m_waters[1] = Water(vec2(1000, 600), 75.f);

	//int screenWidth = 0;
	//int screenHeight = 0;
	//glfwGetWindowSize(m_window, &screenWidth, &screenHeight);
	//int x = rand() % screenWidth;
	//int y = rand() % screenHeight;
	//delete m_water;
	//m_water = new Water(vec2(x, y), 75.f);
}

float GeneticApp::SimulateEnemy(vec2& centre, float range, Agent* agent)
{
	vec2 displacment = agent->GetPosition() - centre;
	float distance = glm::length(displacment);
	if (distance < range)
		return true;
	return false;
}

void GeneticApp::DrawAgents()
{
	//for (int i = 0; i < MAX_AGENTS; i++)
	//	m_agents[i].Draw();

	m_agent->Draw();
}

void GeneticApp::AddAgentGizmos()
{
	//for (int i = 0; i < MAX_AGENTS; i++)
	//	m_agents[i].AddGizmo();

	m_agent->AddGizmo();
}

void GeneticApp::AddEnemyWidgets(float delta)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		m_enemies[i].Update(delta, m_agent);
		m_enemies[i].AddGizmo();
	}
}

void GeneticApp::AddFoodWidgets()
{
	//for (int i = 0; i < MAX_FOOD; i++)
	//	m_foods[i].AddGizmo();

	m_food->AddGizmo();
}

void GeneticApp::AddWaterWidgets()
{
	//for (int i = 0; i < MAX_WATER; i++)
	//	m_waters[i].AddGizmo();

	m_water->AddGizmo();
}