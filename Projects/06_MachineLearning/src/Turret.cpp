#include "Turret.h"
#include "Gizmos.h"

Turret::Turret(glm::vec2 centre, float range)
{
	m_centre = centre;
	m_range = range;
	m_active = true;
}

Turret::Turret()
{
	m_active = false;
}

void Turret::AddGizmo()
{
	if (m_active)
	{
		Gizmos::Add2DCircle(m_centre, m_range, 20, glm::vec4(0.5, 0, 0, 0.3));
	}
}

float Turret::CheckRange(glm::vec2 testPoint)
{
	if (m_active)
	{
		glm::vec2 displacment = testPoint - m_centre;
		float distance = glm::length(displacment);
		if (distance < m_range)
		{
			return 1.f;
		}
	}
	return 0.f;
}