#include "MazeWalls.h"
#include <glm/ext.hpp>
#include "Gizmos.h"

using glm::vec2;

MazeWalls::MazeWalls(vec2 centre, float width, float height, float rotation)
{
	m_centre = centre;
	m_width = width;
	m_height = height;
	m_rotation = rotation;
	m_active = true;
}

MazeWalls::MazeWalls()
{
	m_active = false;
}

void MazeWalls::AddGizmo()
{
	glm::mat4 rotationMatrix;
	rotationMatrix = glm::rotate(rotationMatrix, m_rotation, glm::vec3(0, 0, 1));
	Gizmos::Add2DAABBFilled(m_centre, vec2(m_width, m_height), glm::vec4(0.5, 0, 0, 0.3), &rotationMatrix);
}

/*bool MazeWalls::CheckRange(vec2 testPoint)
{
	if (m_active)
	{
		vec2 normal(-cos(m_rotation), -sin(m_rotation));
		vec2 displacement = testPoint - m_centre;
		float distance = glm::dot(normal, displacement);
		if (distance < 0.f)
			return true;
		else
			return false;

	}
	return false;
}*/

bool MazeWalls::CheckRange(vec2 testPoint, vec2 testVelocity)
{
	if (m_active)
	{
		float left = m_centre.x - m_width;
		float right = m_centre.x + m_width;
		float top = m_centre.y - m_height;
		float bottom = m_centre.y + m_height;
		if ((testPoint.x > left && testVelocity.x > 0) || (testPoint.x < right  && testVelocity.x < 0))
		{
			return true;
		}
		if ((testPoint.y > top && testVelocity.y > 0) || (testPoint.y < bottom  && testVelocity.y < 0))
		{
			return true;
		}
		else
			return false;
	}
	return false;
}