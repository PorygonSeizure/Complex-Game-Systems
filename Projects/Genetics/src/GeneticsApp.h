#ifndef _GENETICS_APP_H_
#define _GENETICS_APP_H_

#include "BaseApp.h"
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

#include <glm/glm.hpp>
#include <vector>
#include <thread>

class AI;
class Obstacle;
class Item;
//class MazeWalls;
class Wall;
class Graph;
class GraphNode;

class GeneticsApp : public BaseApp
{
public:
	GeneticsApp() {};
	virtual ~GeneticsApp() {};

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update(float deltaTime);
	virtual void Draw();

private:
	void SetupSimulation();

	void SetupAI();

	void SetupObstacles();
	void SetupItem();
	void SetupMazeWalls();

	void UpdateAI(float delta);
	void DrawAI();
	void AddAIGizmos();

	void AddWallWidgets();
	void AddObstacleWidgets();
	void AddItemWidgets();

	bool CheckAIWalls(AI* ai);
	void CheckAIDamage(AI* ai);
	void CheckAIItem(AI* ai);

	void LearningFunction();

	float SimulateObstacle(glm::vec2& centre, float range, AI* ai);

	AI* RouletteWheelSelection();

	void CheckForPathfinding();

	static const int MAX_AI = 1;
	static const int MAX_OBSTACLES = 5;
	static const int MAX_ITEMS = 5;
	static const int MAX_WALLS = 20;

	AI* m_ai;
	Obstacle* m_obstacles;
	Item* m_items;
	//MazeWalls* m_mazeWalls;
	Wall* m_walls;

	Graph* m_graph;

	int m_generation;
	int m_fittest;

	std::vector<AI*> m_population;
	std::vector<GraphNode*> m_path;
	std::vector<std::thread> pathLoop;
};

#endif