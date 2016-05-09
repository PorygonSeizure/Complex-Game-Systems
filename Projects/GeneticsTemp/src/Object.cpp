#include "Object.h"
#include "Gizmos.h"

using glm::vec2;

Object::Object(vec2 centre, float range)
{
	m_centre = centre;
	m_range = range;
	m_active = true;
}

bool Object::CheckRange(vec2 testPoint)	//checks if the testPoint is inside the object's space
{
	if (m_active)
	{
		vec2 displacment = testPoint - m_centre;
		float distance = glm::length(displacment);
		if (distance < m_range)
			return true;
	}
	return false;
}