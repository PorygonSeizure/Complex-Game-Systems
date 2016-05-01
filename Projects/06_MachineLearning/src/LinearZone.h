#ifndef _LINEAR_ZONE_H_
#define _LINEAR_ZONE_H_

#include "glm/glm.hpp"

class LinearZone
{
public:
	LinearZone();
	LinearZone(glm::vec2 centre, float rotation);
	~LinearZone() {};

	void AddGizmo();
	float CheckRange(glm::vec2 testPoint);

private:
	glm::vec2 m_centre;
	float m_rotation;
	bool m_active;
};

#endif