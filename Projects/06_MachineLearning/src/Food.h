#ifndef _FOOD_H_
#define _FOOD_H_

#include "glm/glm.hpp"

class Food
{
public:
	Food();
	Food(glm::vec2 centre, float range);
	~Food() {};

	void AddGizmo();
	float CheckRange(glm::vec2 testPoint);

private:
	glm::vec2 m_centre;
	float m_range;
	bool m_active;
};

#endif