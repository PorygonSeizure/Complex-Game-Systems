#include "LinearZone.h"
#include <glm/ext.hpp>
#include "Gizmos.h"

using glm::vec2;

LinearZone::LinearZone(vec2 centre, float rotation)
{
	m_centre = centre;
	m_rotation = rotation;
	m_active = true;
}

LinearZone::LinearZone()
{
	m_active = false;
}

void LinearZone::AddGizmo()
{
	float length = 1000.f;
	vec2 offset(length * cos(m_rotation), length * sin(m_rotation));
	vec2 centre;
	glm::mat4 rotationMatrix;
	rotationMatrix = glm::rotate(rotationMatrix, m_rotation, glm::vec3(0, 0, 1));
	Gizmos::Add2DAABBFilled(m_centre + offset, vec2(length, length), glm::vec4(0.5, 0, 0, 0.3), &rotationMatrix);
}

float LinearZone::CheckRange(vec2 testPoint)
{
	if (m_active)
	{
		vec2 normal(-cos(m_rotation), -sin(m_rotation));
		vec2 displacement = testPoint - m_centre;
		float distance = glm::dot(normal, displacement);
		if (distance < 0.f)
			return 1.f;
		else
			return 0.f;

	}
	return 0.f;
}