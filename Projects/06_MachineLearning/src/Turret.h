#ifndef _TURRET_H_
#define _TURRET_H_

#include "glm/glm.hpp"

class Turret
{
public:
	Turret();
	Turret(glm::vec2 centre, float range);
	~Turret() {};

	void AddGizmo();
	float CheckRange(glm::vec2 testPoint);

private:
	glm::vec2 m_centre;
	float m_range;
	bool m_active;
};

#endif