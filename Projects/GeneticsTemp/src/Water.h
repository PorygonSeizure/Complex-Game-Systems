#ifndef _WATER_H_
#define _WATER_H_

#include "glm/glm.hpp"
#include "Object.h"

class Water : public Object
{
public:
	Water() { m_active = false; }
	Water(glm::vec2 centre, float range);
	~Water() {};

	void AddGizmo();
};

#endif