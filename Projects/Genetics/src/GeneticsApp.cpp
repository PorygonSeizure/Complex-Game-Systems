#include "GeneticsApp.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "Gizmos.h"

#include "Genetic/AI.h"
#include "Genetic/Obstacle.h"
#include "Genetic/Item.h"
//#include "Genetic/MazeWalls.h"
#include "Genetic/Walls.h"
#include "Graph.h"
#include "GraphNode.h"

using glm::vec4;
using glm::mat4;
using glm::vec2;

bool GeneticsApp::Startup()
{
	//create a basic window
	CreateGLFWWindow("AIE OpenGL Application", 1280, 720);

	//start the gizmo system that can draw basic shapes
	Gizmos::Create(0xffff, 0xffff, 0xffff, 0xffff);

	m_ai = new AI[MAX_AI];
	m_obstacles = new Obstacle[MAX_OBSTACLES];
	m_items = new Item[MAX_ITEMS];
	//m_mazeWalls = new MazeWalls[MAX_WALLS];
	m_walls = new Wall[MAX_WALLS];

	m_graph = new Graph();
	m_graph->CreateGrid(64, 64, 1280, 720);

	ImGui_ImplGlfwGL3_Init(m_window, true);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = 1280.f;
	io.DisplaySize.y = 720.f;

	SetupSimulation();

	pathLoop.push_back(std::thread(&AI::BitsetMovement, &m_ai[0], m_graph));

	return true;
}

void GeneticsApp::Shutdown()
{
	for (auto& thread : pathLoop)
		thread.join();
	Gizmos::Destroy();

	//destroy our window properly
	DestroyGLFWWindow();
}

bool GeneticsApp::Update(float deltaTime)
{
	//close the application if the window closes
	if (glfwWindowShouldClose(m_window) || glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	//clear the gizmos out for this frame
	Gizmos::Clear();

	UpdateAI(deltaTime);
	AddAIGizmos();
	AddWallWidgets();
	AddObstacleWidgets();
	AddItemWidgets();

	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
	{
		pathLoop.push_back(std::thread(&GeneticsApp::CheckForPathfinding, this));
	}

	//return true, else the application closes
	return true;
}

void GeneticsApp::Draw()
{
	//static float wallX = m_mazeWalls[0].GetCentre().x;
	//static float wallY = m_mazeWalls[0].GetCentre().y;
	//static float wallW = m_mazeWalls[0].GetWidth();
	//static float wallH = m_mazeWalls[0].GetHeight();
	//static float wallR = m_mazeWalls[0].GetRotation();
	//static float itemX = m_items[0].GetCentre().x;
	//static float itemY = m_items[0].GetCentre().y;
	static int itemCol = m_items[0].GetNode()->GetLoc().x;
	static int itemRow = m_items[0].GetNode()->GetLoc().y;
	static float itemRad = m_items[0].GetRange();
	//static float obstacleX = m_obstacles[0].GetCentre().x;
	//static float obstacleY = m_obstacles[0].GetCentre().y;
	static int obstacleRow = m_obstacles[0].GetNode()->GetLoc().x;
	static int obstacleCol = m_obstacles[0].GetNode()->GetLoc().y;
	static float obstacleRad = m_obstacles[0].GetRange();

	//clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplGlfwGL3_NewFrame();



	//get a orthographic projection matrix and draw 2D gizmos
	int width = 0;
	int height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	mat4 guiMatrix = glm::ortho<float>(0.f, (float)width, 0.f, (float)height);

	Gizmos::Draw2D(guiMatrix);

	if (ImGui::Begin("My rendering options"))
	{
		//ImGui::SliderFloat("Item X", &itemX, 0.f, (float)width);
		//ImGui::SliderFloat("Item Y", &itemY, 0.f, (float)height);
		ImGui::SliderInt("Item Row", &itemRow, 0, 63);
		ImGui::SliderInt("Item Col", &itemCol, 0, 63);
		ImGui::SliderFloat("Item Radius", &itemRad, 0.f, 500.f);
		//ImGui::SliderFloat("Obstacle X", &obstacleX, 0.f, (float)width);
		//ImGui::SliderFloat("Obstacle Y", &obstacleY, 0.f, (float)height);
		ImGui::SliderInt("Obstacle Row", &obstacleRow, 0, 63);
		ImGui::SliderInt("Obstacle Col", &obstacleCol, 0, 63);
		ImGui::SliderFloat("Obstacle Radius", &obstacleRad, 0.f, 500.f);
		//ImGui::SliderFloat("Wall X", &wallX, 0.f, (float)width);
		//ImGui::SliderFloat("Wall Y", &wallY, 0.f, (float)height);
		//ImGui::SliderFloat("Wall Width", &wallW, 0.f, 1000.f);
		//ImGui::SliderFloat("Wall Height", &wallH, 0.f, 1000.f);
		//ImGui::SliderFloat("Wall Rotation", &wallR, 0.f, glm::two_pi<float>());
	}
	ImGui::End();

	ImGui::Render();

	//if (itemX != m_items[0].GetCentre().x || itemY != m_items[0].GetCentre().y)
	//	m_items[0].SetCentre(vec2(itemX, itemY));
	if (m_graph->GetNode(itemCol, itemRow) != m_graph->GetClosestNode(m_items[0].GetCentre()))
		m_items[0].SetCentre(m_graph->GetNode(itemCol, itemRow)->GetPos());
	if (itemRad != m_items[0].GetRange())
		m_items[0].SetRange(itemRad);

	//if (obstacleX != m_obstacles[0].GetCentre().x || obstacleY != m_obstacles[0].GetCentre().y)
	//	m_obstacles[0].SetCentre(vec2(obstacleX, obstacleY));
	if (m_graph->GetNode(obstacleCol, obstacleRow) != m_graph->GetClosestNode(m_obstacles[0].GetCentre()))
		m_obstacles[0].SetCentre(m_graph->GetNode(obstacleCol, obstacleRow)->GetPos());
	if (obstacleRad != m_obstacles[0].GetRange())
		m_obstacles[0].SetRange(obstacleRad);

	//if (wallX != m_mazeWalls[0].GetCentre().x || wallY != m_mazeWalls[0].GetCentre().y)
	//	m_mazeWalls[0].SetCentre(vec2(wallX, wallY));
	//if (wallW != m_mazeWalls[0].GetWidth())
	//	m_mazeWalls[0].SetWidth(wallW);
	//if (wallH != m_mazeWalls[0].GetHeight())
	//	m_mazeWalls[0].SetHeight(wallH);
	//if (wallR != m_mazeWalls[0].GetRotation())
	//	m_mazeWalls[0].SetRotation(wallR);
}

void GeneticsApp::CheckForPathfinding()
{
	m_graph->SetCurrentNode(m_ai[0].GetNode());

	for (int i = 0; i < MAX_ITEMS; i++)
	{
		if (m_items[0].GetActive())
		{
			m_graph->Dijkstra(m_items[i].GetNode());
			m_graph->SetCurrentNode(m_items[i].GetNode());
		}
	}

	m_path = m_graph->BuildPath(m_items[4].GetNode());
}

void GeneticsApp::SetupSimulation()
{
	SetupAI();
	SetupMazeWalls();
	SetupObstacles();
	SetupItem();
}

void GeneticsApp::SetupMazeWalls()
{
	int tempWidth = 0;
	int tempHeight = 0;
	glfwGetWindowSize(m_window, &tempWidth, &tempHeight);
	float width = (float)tempWidth;
	float height = (float)tempHeight;
	m_walls[0] = Wall(m_graph->GetRow(0, 63, 0), 10.f, 64);
	m_walls[1] = Wall(m_graph->GetRow(26, 36, 4), 10.f, 11);
	m_walls[2] = Wall(m_graph->GetRow(0, 11, 5), 10.f, 12);
	m_walls[3] = Wall(m_graph->GetRow(38, 55, 5), 10.f, 18);
	//m_mazeWalls[0] = MazeWalls(vec2(10.f, height / 2.f), 10.f, height / 2.f, 0.f);
	//m_mazeWalls[1] = MazeWalls(vec2(width / 2.f, 10.f), width / 2.f, 10.f, 0.f);
	//m_mazeWalls[2] = MazeWalls(vec2(width - 10.f, height / 2.f), 10.f, height / 2.f, 0.f);
	//m_mazeWalls[3] = MazeWalls(vec2(width / 2.f, height - 10.f), width / 2.f, 10.f, 0.f);

	//m_mazeWalls[0] = MazeWalls(vec2(600, 400), 10.f, 100.f, 0.f);
}

void GeneticsApp::SetupAI()
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(m_window, &screenWidth, &screenHeight);

	srand((unsigned int)time(NULL));
	for (int index = 0; index < MAX_AI; index++)
	{
		//vec2 startPos;
		//startPos.x = glm::clamp((float)(rand() % screenWidth), 15.f, screenWidth - 15.f);
		//startPos.y = glm::clamp((float)(rand() % 2 * screenHeight), 15.f, screenHeight - 15.f);
		float size = 30.f;
		float facing = 44.f / 7.f * (float)((rand() % 1000) / 1000.f);
		m_ai[index].Setup(m_graph->GetNode(1, 1), size, vec4(1, 1, 1, 1), facing);
	}
}

void GeneticsApp::UpdateAI(float delta)
{
	for (int i = 0; i < MAX_AI; i++)
	{
		m_ai[i].Update(delta);
		CheckAIDamage(&m_ai[i]);
		CheckAIItem(&m_ai[i]);
	}
}

bool GeneticsApp::CheckAIWalls(AI* ai)
{
	for (int i = 0; i < MAX_WALLS; i++)
	{
		//if (m_mazeWalls[i].CheckRange(ai->GetPosition(), ai->GetVelocity()))
		if (m_walls[i].CheckRange(ai->GetNode()))
			return true;
	}

	return false;
}

void GeneticsApp::CheckAIDamage(AI* ai)
{
	bool colision = false;
	//for (int i = 0; i < MAX_OBSTACLES; i++)
	//{
	//	if (m_obstacles[i].CheckRange(ai->GetPosition()))
	//	{
	//		colision = true;
	//		break;
	//	}
	//}

	for (int i = 0; i < MAX_OBSTACLES; i++)
	{
		if (m_obstacles[i].CheckNode(ai->GetNode()))
		{
			colision = true;
			break;
		}
	}

	ai->HurtAI(colision);
}

void GeneticsApp::CheckAIItem(AI* ai)
{
	bool colision = false;
	for (int i = 0; i < MAX_ITEMS; i++)
	{
		if (m_items[i].CheckRange(ai->GetPosition()))
		{
			colision = true;
			break;
		}
	}

	ai->FeedAI(colision);
}

void GeneticsApp::SetupObstacles()
{
	m_obstacles[0] = Obstacle(m_graph->GetNode(0, 0), 10.f);
}

void GeneticsApp::SetupItem()
{
	m_items[0] = Item(m_graph->GetNode(29, 2), 10.f);
	m_items[1] = Item(m_graph->GetNode(32, 23), 10.f);
	m_items[2] = Item(m_graph->GetNode(8, 27), 10.f);
	m_items[3] = Item(m_graph->GetNode(50, 30), 10.f);
	m_items[4] = Item(m_graph->GetNode(17, 50), 10.f);
}

float GeneticsApp::SimulateObstacle(vec2& centre, float range, AI* ai)
{
	vec2 displacment = ai->GetPosition() - centre;
	float distance = glm::length(displacment);
	if (distance < range)
		return true;

	return false;
}

void GeneticsApp::DrawAI()
{
	for (int i = 0; i < MAX_AI; i++)
		m_ai[i].Draw();
}

void GeneticsApp::AddAIGizmos()
{
	for (int i = 0; i < MAX_AI; i++)
		m_ai[i].AddGizmo();
}

void GeneticsApp::AddWallWidgets()
{
	for (int i = 0; i < MAX_WALLS; i++)
		//m_mazeWalls[i].AddGizmo();
		m_walls[i].AddGizmo();
}

void GeneticsApp::AddObstacleWidgets()
{
	for (int i = 0; i < MAX_OBSTACLES; i++)
		m_obstacles[i].AddGizmo();
}

void GeneticsApp::AddItemWidgets()
{
	for (int i = 0; i < MAX_ITEMS; i++)
		m_items[i].AddGizmo();
}

AI* GeneticsApp::RouletteWheelSelection()
{
	int totalFitness = 0;
	for (auto& ai : m_population)
		totalFitness += ai->GetFitness();

	int selection = rand() % totalFitness;
	totalFitness = 0;

	for (auto ai : m_population)
	{
		totalFitness += ai->GetFitness();
		if (selection < totalFitness)
			return ai;
	}

	return nullptr;
}

void GeneticsApp::LearningFunction()
{
	for (auto& ai : m_population)
	{
		int score = ai->GetFitness();
		if (score < m_fittest)
			m_fittest = score;
	}

	if (m_fittest == 0)
		return;

	++m_generation;

	std::vector<AI*> nextGeneration;

	while (m_population.size() != nextGeneration.size())
	{
		//selection
		auto iterator = RouletteWheelSelection();
		AI* parent1 = iterator;

		iterator = RouletteWheelSelection();
		AI* parent2 = iterator;

		AI* sibling1;
		AI* sibling2;

		//crossover
		if (rand() % 1000 < 700)
		{
			for (int i = 0; i < 256; ++i)
				sibling1->GetBitset()[i] = i < 128 ? parent1->GetBitset()[i] : parent2->GetBitset()[i];

			for (int i = 0; i < 256; ++i)
				sibling2->GetBitset()[i] = i < 128 ? parent2->GetBitset()[i] : parent1->GetBitset()[i];
		}
		else
		{
			sibling1 = parent1;
			sibling2 = parent2;
		}

		//mutation
		for (unsigned int i = 0; i < sibling1->GetBitset().size(); ++i)
		{
			if (rand() % 1000 < 5)
				sibling1->Flip(i);
		}
		for (unsigned int i = 0; i < sibling2->GetBitset().size(); ++i)
		{
			if (rand() % 1000 < 5)
				sibling2->Flip(i);
		}

		//populate
		nextGeneration.push_back(sibling1);
		nextGeneration.push_back(sibling2);
	}

	m_population = nextGeneration;
}