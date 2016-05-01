#ifndef _ITEM_H_
#define _ITEM_H_

#include "glm/glm.hpp"

class GraphNode;
class Graph;

class Item
{
public:
	Item();
	Item(glm::vec2 centre, float range, Graph* graph);
	Item(GraphNode* node, float range);
	~Item() {};

	void SetCentre(glm::vec2 centre) { m_centre = centre; }
	void SetRange(float range) { m_range = range; }
	void SetNode(GraphNode* node) { m_node = node; }
	glm::vec2 GetCentre() { return m_centre; }
	float GetRange() { return m_range; }
	GraphNode* GetNode() { return m_node; }

	bool GetActive() { return m_active; }

	void AddGizmo();
	bool CheckRange(glm::vec2 testPoint);
	bool CheckRange(GraphNode* testNode);

private:
	glm::vec2 m_centre;
	float m_range;
	bool m_active;
	GraphNode* m_node;
};

#endif