#ifndef _WATER_H_
#define _WATER_H_

#include "glm/glm.hpp"

class Water
{
public:
	Water();
	Water(glm::vec2 centre, float range);
	~Water() {};

	void AddGizmo();
	float CheckRange(glm::vec2 testPoint);

private:
	glm::vec2 m_centre;
	float m_range;
	bool m_active;
};

#endif