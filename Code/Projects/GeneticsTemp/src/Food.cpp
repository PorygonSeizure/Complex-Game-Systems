#include "Food.h"
#include "Gizmos.h"

using glm::vec2;

Food::Food(vec2 centre, float range)
{
	m_centre = centre;
	m_range = range;
	m_active = true;
}

void Food::AddGizmo()	//draws the food item using gizmos
{
	if (m_active)
		Gizmos::Add2DCircle(m_centre, m_range, 20, glm::vec4(0, 1, 0, 0.25f));
}