#include "Item.h"
#include "Gizmos.h"
#include "../GraphNode.h"
#include "../Graph.h"

using glm::vec2;

Item::Item(vec2 centre, float range, Graph* graph)
{
	m_centre = centre;
	m_range = range;
	m_node = graph->GetClosestNode(centre);
	m_active = true;
}

Item::Item(GraphNode* node, float range)
{
	m_range = range;
	m_node = node;
	m_centre = node->GetPos();
	m_active = true;
}

Item::Item()
{
	m_active = false;
}

void Item::AddGizmo()
{
	if (m_active)
	{
		Gizmos::Add2DCircle(m_centre, m_range, 20, glm::vec4(0, 1, 0, 0.25f));
	}
}

bool Item::CheckRange(vec2 testPoint)
{
	if (m_active)
	{
		vec2 displacment = testPoint - m_centre;
		float distance = glm::length(displacment);
		if (distance < m_range)
		{
			return true;
		}
	}
	return false;
}

bool Item::CheckRange(GraphNode* testNode)
{
	if (m_active)
	{
		vec2 displacment = testNode->GetPos() - m_centre;
		float distance = glm::length(displacment);
		if (distance < m_range)
		{
			return true;
		}
	}
	return false;
}
