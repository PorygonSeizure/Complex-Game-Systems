#ifndef _WALLS_H_
#define _WALLS_H_

#include "glm/glm.hpp"

class GraphNode;
class Graph;

class Wall
{
public:
	Wall();
	Wall(GraphNode** nodes, float range, int wallSize);
	~Wall() { /*delete[] m_nodes;*/ }

	void SetRange(float range) { m_range = range; }
	void SetNode(GraphNode** nodes, int wallSize) { m_nodes = nodes; m_wallSize = wallSize; }
	float GetRange() { return m_range; }
	GraphNode** GetNode() { return m_nodes; }

	void AddGizmo();
	bool CheckRange(GraphNode* testNode);

private:
	float m_range;
	bool m_active;
	GraphNode** m_nodes;

	float m_nodeWidth;
	float m_nodeHeight;
	int m_wallSize;
};

#endif