#pragma once

#include "glm/glm.hpp"

class LinearZone
{
private:
	glm::vec2 _centre;
	float _rotation;
	bool active;
public:
	LinearZone();
	LinearZone(glm::vec2, float range);
	~LinearZone();
	void addGizmo();
	float checkRange(glm::vec2 testPoint);
};