#ifndef _BOUNDING_SPHERE_H_
#define _BOUNDING_SPHERE_H_

#include <glm/glm.hpp>

class BoundingSphere
{
public:
	BoundingSphere() : m_centre(0), m_radius(1) {}
	~BoundingSphere() {}

	void Fit(const BoundingSphere& sphere)
	{
		float d = glm::distance(m_centre, sphere.m_centre) + sphere.m_radius;

		if (d > m_radius)
			m_radius = d;
	}

	glm::vec4 m_centre;
	float m_radius;
};

#endif