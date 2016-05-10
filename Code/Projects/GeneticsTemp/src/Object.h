#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "glm/glm.hpp"

class Object
{
public:
	Object() { m_active = false; }
	Object(glm::vec2 centre, float range);
	virtual ~Object() {};

	virtual void AddGizmo() = 0 {};

	bool CheckRange(glm::vec2 testPoint);

	glm::vec2 GetPos() { return m_centre; }

	void SetActive(bool active) { m_active = active; }

protected:
	glm::vec2 m_centre;

	float m_range;

	bool m_active;
};

#endif