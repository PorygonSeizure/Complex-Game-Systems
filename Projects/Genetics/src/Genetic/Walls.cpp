#include "Walls.h"
#include "Gizmos.h"
#include "../GraphNode.h"
#include "../Graph.h"

Wall::Wall(GraphNode** nodes, float range, int wallSize)
{
	m_range = range;
	m_nodes = nodes;
	m_active = true;
	m_wallSize = wallSize;
}

Wall::Wall()
{
	m_active = false;
}

void Wall::AddGizmo()
{
	if (m_active)
	{
		for (int i = 0; i < m_wallSize; i++)
		{
			Gizmos::Add2DAABBFilled(m_nodes[i]->GetPos(), glm::vec2(/*m_nodeWidth, m_nodeHeight*/10.f, 10.f), glm::vec4(0.5, 0, 0, 0.3));
		}
	}
}

bool Wall::CheckRange(GraphNode* testNode)
{
	if (m_active)
	{
		for (int i = 0; i < m_wallSize; i++)
		{
			glm::vec2 displacment = testNode->GetPos() - m_nodes[i]->GetPos();;
			float distance = glm::length(displacment);
			if (distance < m_range)
			{
				return true;
			}
		}
	}
	return false;
}