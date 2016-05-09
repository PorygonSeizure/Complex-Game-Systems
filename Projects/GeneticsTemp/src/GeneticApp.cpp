#include "GeneticApp.h"
#include <imgui.h>
#include "ImGui/imgui_impl_glfw_gl3.h"

#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

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
	m_enemies = new Enemy[MAX_ENEMY];

	m_agent = new Agent();
	m_food = new Food();
	m_water = new Water();
}

bool GeneticApp::Startup()	//startup the project
{
	//create a basic window
	CreateGLFWWindow("AIE OpenGL Application", 1280, 720);

	//start the gizmo system that can draw basic shapes
	Gizmos::Create(0xffff, 0xffff, 0xffff, 0xffff);

	//initiliaze imgui
	ImGui_ImplGlfwGL3_Init(m_window, true);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = 1280.f;
	io.DisplaySize.y = 720.f;
	
	SetUpSimulation();

	return true;
}

void GeneticApp::Shutdown()	//destroy all objects before program closes
{
	delete m_agent;
	delete m_food;
	delete m_water;
	delete[] m_enemies;

	Gizmos::Destroy();

	//destroy our window properly
	DestroyGLFWWindow();
}

bool GeneticApp::Update(float deltaTime)	//update the screen
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

void GeneticApp::Draw()	//draw the screen
{
	//clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw imgui
	ImGui_ImplGlfwGL3_NewFrame();

	//get a orthographic projection matrix and draw 2D gizmos
	int width = 0;
	int height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	mat4 guiMatrix = glm::ortho<float>(0.f, (float)width, 0.f, (float)height);

	Gizmos::Draw2D(guiMatrix);

	vec3 weights = m_agent->GetWeights();
	int gen = m_agent->GetGen();

	if (ImGui::Begin("My rendering options"))
	{
		ImGui::SliderFloat("Base Food Weight", &m_agent->m_baseFoodWeight, -300.f, 300.f);
		ImGui::SliderFloat("Base Water Weight", &m_agent->m_baseWaterWeight, -300.f, 300.f);
		ImGui::SliderFloat("Base Enemy Weight", &m_agent->m_baseEnemyWeight, -300.f, 300.f);
		ImGui::Text("Hunger Level %f", m_agent->m_hunger);
		ImGui::Text("Thrist Level %f", m_agent->m_thrist);
		ImGui::Text("1st Best Gen: %d", m_agent->GetBestGens()[0].generation);
		ImGui::Text("2nd Best Gen: %d", m_agent->GetBestGens()[1].generation);
		ImGui::Text("3rd Best Gen: %d", m_agent->GetBestGens()[2].generation);
		if (!m_agent->foodValues.empty() && !m_agent->waterValues.empty() && !m_agent->enemyValues.empty() && !m_agent->times.empty() && ImGui::CollapsingHeader("Generation Graphs"))
		{
			ImGui::PlotLines("##Food", m_agent->foodValues.data(), m_agent->foodValues.size(), 0, NULL, 0.f, 300.f, ImVec2(0, 80));
			ImGui::PlotLines("##Water", m_agent->waterValues.data(), m_agent->waterValues.size(), 0, NULL, 0.f, 300.f, ImVec2(0, 80));
			ImGui::PlotLines("##Enemy", m_agent->enemyValues.data(), m_agent->enemyValues.size(), 0, NULL, -150.f, 150.f, ImVec2(0, 80));
			ImGui::PlotLines("##Time", m_agent->times.data(), m_agent->times.size(), 0, NULL, 0.f, 30.f, ImVec2(0, 80));
		}
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
			}
		}
	}
	ImGui::End();

	ImGui::Render();
}

void GeneticApp::SetUpSimulation()	//setup all the object
{
	SetUpAgents();
	SetUpEnemy();
	SetUpFood();
	SetUpWater();
}

void GeneticApp::SetUpAgents()	//setup new agent
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(m_window, &screenWidth, &screenHeight);

	srand((unsigned int)time(NULL));
	vec2 startPos;
	startPos.x = (float)(rand() % screenWidth);
	startPos.y = (float)(rand() % 2 * screenHeight);
	float size = 30.f;
	float facing = 44.f / 7.f * (float)((rand() % 1000) / 1000.f);
	m_agent->Setup(startPos, size, vec4(1, 1, 1, 1), facing);
}

void GeneticApp::UpdateAgents(float delta)	//update the agent and check if it's come into contact with any other object
{
	m_agent->Update(delta, m_food, m_water, m_enemies, MAX_ENEMY);
	CheckAgentDamage(m_agent);
	CheckAgentFood(m_agent);
	CheckAgentWater(m_agent);
}

void GeneticApp::CheckAgentDamage(Agent* agent)	//check if agent has come into contact with an enemy
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (m_enemies[i].CheckRange(agent->GetPosition()))
		{
			m_memory.push_back(agent->EnemyContact());
			SetUpEnemy();
			SetUpFood();
			SetUpWater();
			return;
		}
	}
}

void GeneticApp::CheckAgentFood(Agent* agent)		//check if agent has come into contact with food
{
	if (m_food->CheckRange(agent->GetPosition()))
	{
		agent->FoodContact();
		SetUpFood();
	}
	if (agent->m_hunger < 1)
	{
		m_memory.push_back(agent->FoodDeath());
		SetUpEnemy();
		SetUpFood();
		SetUpWater();
	}
}

void GeneticApp::CheckAgentWater(Agent* agent)	//check if agent has come into contact with water
{
	if (m_water->CheckRange(agent->GetPosition()))
	{
		agent->WaterContact();
		SetUpWater();
	}
	if (agent->m_thrist < 1)
	{
		m_memory.push_back(agent->WaterDeath());
		SetUpEnemy();
		SetUpFood();
		SetUpWater();
	}
}

void GeneticApp::SetUpEnemy()	//setup new enemy
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(m_window, &screenWidth, &screenHeight);
	for (int i = 0; i < MAX_ENEMY; i++)
		m_enemies[i] = Enemy(vec2(rand() % screenWidth, rand() % screenHeight), 25.f);
}

void GeneticApp::SetUpFood()	//setup new food
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(m_window, &screenWidth, &screenHeight);
	int x = rand() % screenWidth;
	int y = rand() % screenHeight;
	delete m_food;
	m_food = new Food(vec2(x, y), 75.f);
}

void GeneticApp::SetUpWater()	//setup new water
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(m_window, &screenWidth, &screenHeight);
	int x = rand() % screenWidth;
	int y = rand() % screenHeight;
	delete m_water;
	m_water = new Water(vec2(x, y), 75.f);
}

void GeneticApp::AddEnemyWidgets(float delta)	//update all enemies
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		m_enemies[i].Update(delta, m_agent);
		m_enemies[i].AddGizmo();
	}
}

void GeneticApp::AddAgentGizmos() { m_agent->AddGizmo(); }	//draw agents to screen
void GeneticApp::AddFoodWidgets() { m_food->AddGizmo(); }	//draw food item to screen
void GeneticApp::AddWaterWidgets() { m_water->AddGizmo(); }	//draw water item to screen