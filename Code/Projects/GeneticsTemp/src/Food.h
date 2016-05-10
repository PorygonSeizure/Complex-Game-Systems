#ifndef _FOOD_H_
#define _FOOD_H_

#include "glm/glm.hpp"
#include "Object.h"

class Food : public Object
{
public:
	Food() { m_active = false; }
	Food(glm::vec2 centre, float range);
	virtual ~Food() {};

	void AddGizmo();
};

#endif