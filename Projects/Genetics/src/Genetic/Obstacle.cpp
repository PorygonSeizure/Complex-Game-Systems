#include "Obstacle.h"
#include "Gizmos.h"
#include "../GraphNode.h"
#include "../Graph.h"

Obstacle::Obstacle(glm::vec2 centre, float range, Graph* graph)
{
	m_centre = centre;
	m_range = range;
	m_node = graph->GetClosestNode(centre);
	m_active = true;
}

Obstacle::Obstacle(GraphNode* node, float range)
{
	m_range = range;
	m_node = node;
	m_centre = node->GetPos();
	m_active = true;
}

Obstacle::Obstacle()
{
	m_active = false;
}

void Obstacle::AddGizmo()
{
	if (m_active)
	{
		Gizmos::Add2DCircle(m_centre, m_range, 20, glm::vec4(0.5, 0, 0, 0.3));
	}
}

bool Obstacle::CheckRange(glm::vec2 testPoint)
{
	if (m_active)
	{
		glm::vec2 displacment = testPoint - m_centre;
		float distance = glm::length(displacment);
		if (distance < m_range)
		{
			return true;
		}
	}
	return false;
}

bool Obstacle::CheckNode(GraphNode* testNode)
{
	if (m_active && m_node == testNode)
	{
		return true;
	}
	return false;
}