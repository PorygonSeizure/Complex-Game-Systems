#pragma once

#include "glm/glm.hpp"

class Food
{
private:
	glm::vec2 _centre;
	float _range;
	bool active;
public:
	Food();
	Food(glm::vec2, float range);
	~Food();
	void addGizmo();
	float checkRange(glm::vec2 testPoint);
};