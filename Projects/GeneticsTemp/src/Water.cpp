#include "Water.h"
#include "Gizmos.h"

using glm::vec2;

Water::Water(vec2 centre, float range)
{
	m_centre = centre;
	m_range = range;
	m_active = true;
}

void Water::AddGizmo()
{
	if (m_active)
		Gizmos::Add2DCircle(m_centre, m_range, 20, glm::vec4(0, 0, 1, 0.25f));
}