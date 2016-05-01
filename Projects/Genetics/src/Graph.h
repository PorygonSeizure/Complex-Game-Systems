#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>
#include <glm/glm.hpp>

class GraphNode;

struct GraphVertex
{
	glm::vec2 position;
	GraphNode* node;
};

class Graph
{
public:
	Graph();
	~Graph();

	void AddNode(GraphNode* node);
	void RemoveNode(GraphNode* node);
	void ResetVisited();
	void ResetVisitedAlt();
	void CreateGrid(unsigned int cols, unsigned int rows, float screenWidth, float screenHeight);
	void Dijkstra(GraphNode* end);
	void Ax(GraphNode* end);
	void SetCurrentNode(GraphNode* newCurrentNode);
	void SetNodeVector(std::vector<GraphNode*> path);

	bool SearchDFS(GraphNode* start, GraphNode* end);
	bool SearchBFS(GraphNode* start, GraphNode* end);

	int NodeCount(GraphNode* firstNode, GraphNode* secondNode);
	
	GraphNode* GetClosestNode(glm::vec2 position);
	GraphNode* GetCurrentNode();
	GraphNode* GetNode(int row, int col);

	std::vector<GraphNode*> BuildPath(GraphNode* goal);

	GraphNode** GetRow(int begin, int end, int col);
	GraphNode** GetCol(int begin, int end, int row);
	GraphNode** GetGrid();

	glm::vec2 GetGridDimentions();
	
	std::vector<GraphNode*> GetNodeVector();

private:
	std::vector<GraphNode*> m_nodes;
	GraphNode** m_grid;
	GraphNode* m_currentNode;
	int m_gridCols;
	int m_gridRows;
};

#endif