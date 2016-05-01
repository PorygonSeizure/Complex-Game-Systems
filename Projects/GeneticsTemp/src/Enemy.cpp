#include "Enemy.h"
#include "Gizmos.h"
#include "Agent.h"

#include <GLFW/glfw3.h>

using glm::vec2;

Enemy::Enemy(vec2 centre, float range, float direction)
{
	m_centre = centre;
	m_range = range;
	m_active = true;

	m_maxSpeed = 500.f;
	m_velocity.x = m_maxSpeed * sin(direction);
	m_velocity.y = m_maxSpeed * cos(direction);
}

void Enemy::Update(float deltaTime, Agent* agent)
{
	Behaviour(deltaTime, agent);
	m_centre += m_velocity * deltaTime;
	if (CheckBounds())
	{
		if (m_velocity.x > m_maxSpeed)
			m_velocity.x = m_maxSpeed;
		if (m_velocity.x < -m_maxSpeed)
			m_velocity.x = -m_maxSpeed;
		if (m_velocity.y > m_maxSpeed)
			m_velocity.y = m_maxSpeed;
		if (m_velocity.y < -m_maxSpeed)
			m_velocity.y = -m_maxSpeed;
	}
}

bool Enemy::CheckBounds()
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &screenWidth, &screenHeight);

	if (m_centre.x < 0 && m_velocity.x < 0)
	{
		m_velocity.x = -m_velocity.x;
		return true;
	}
	if (m_centre.y < 0 && m_velocity.y < 0)
	{
		m_velocity.y = -m_velocity.y;
		return true;
	}
	if (m_centre.x > screenWidth  && m_velocity.x > 0)
	{
		m_velocity.x = -m_velocity.x;
		return true;
	}
	if (m_centre.y > screenHeight  && m_velocity.y > 0)
	{
		m_velocity.y = -m_velocity.y;
		return true;
	}
	return false;
}

void Enemy::AddGizmo()
{
	if (m_active)
		Gizmos::Add2DCircle(m_centre, m_range, 20, glm::vec4(0.5, 0, 0, 0.3));
}

void Enemy::Behaviour(float deltaTime, Agent* agent)
{
	vec2 temp = glm::normalize(agent->GetPosition() - m_centre) * m_maxSpeed;
	vec2 force = temp - m_velocity;
	force /= 6.f;

	if (force.x >= m_maxSpeed)
		force.x = m_maxSpeed;
	if (force.y >= m_maxSpeed)
		force.y = m_maxSpeed;
	if (force.x <= -m_maxSpeed)
		force.x = -m_maxSpeed;
	if (force.y <= -m_maxSpeed)
		force.y = -m_maxSpeed;
	m_velocity += force * deltaTime;
}