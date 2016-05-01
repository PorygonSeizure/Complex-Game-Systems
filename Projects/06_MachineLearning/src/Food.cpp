#include "Food.h"
#include "Gizmos.h"

using glm::vec2;

Food::Food(vec2 centre, float range)
{
	m_centre = centre;
	m_range = range;
	m_active = true;
}

Food::Food()
{
	m_active = false;
}

void Food::AddGizmo()
{
	if (m_active)
	{
		Gizmos::Add2DCircle(m_centre, m_range, 20, glm::vec4(0, 1, 0, 0.25f));
	}
}

float Food::CheckRange(vec2 testPoint)
{
	if (m_active)
	{
		vec2 displacment = testPoint - m_centre;
		float distance = glm::length(displacment);
		if (distance < m_range)
		{
			return 1;
		}
	}
	return 0;
}