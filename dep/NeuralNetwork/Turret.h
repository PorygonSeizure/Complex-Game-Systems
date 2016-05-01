#pragma once

#include "glm/glm.hpp"

class Turret
{
private:
	glm::vec2 _centre;
	float _range;
	bool active;
public:
	Turret();
	Turret(glm::vec2,float range);
	~Turret();
	void addGizmo();
	float checkRange(glm::vec2 testPoint);
};