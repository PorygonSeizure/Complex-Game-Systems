#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "glm/glm.hpp"
#include "Object.h"

class Agent;

class Enemy : public Object
{
public:
	Enemy() { m_active = false; }
	Enemy(glm::vec2 centre, float range);
	virtual ~Enemy() {};

	void Update(float deltaTime, Agent* agent);
	void AddGizmo();
	void Behaviour(float deltaTime, Agent* agent);

private:
	bool CheckBounds();

	float m_maxSpeed;

	glm::vec2 m_velocity;
};

#endif