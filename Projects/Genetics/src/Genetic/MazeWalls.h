#ifndef _MAZE_WALLS_H_
#define _MAZE_WALLS_H_

#include "glm/glm.hpp"

class MazeWalls
{
public:
	MazeWalls();
	MazeWalls(glm::vec2 centre, float width, float height, float rotation);
	~MazeWalls() {};

	void SetCentre(glm::vec2 centre) { m_centre = centre; }
	void SetWidth(float width) { m_width = width; }
	void SetHeight(float height) { m_height = height; }
	void SetRotation(float rotation) { m_rotation = rotation; }
	glm::vec2 GetCentre() { return m_centre; }
	float GetWidth() { return m_width; }
	float GetHeight() { return m_height; }
	float GetRotation() { return m_rotation; }

	void AddGizmo();
	bool CheckRange(glm::vec2 testPoint, glm::vec2 testVelocity);

private:
	glm::vec2 m_centre;
	float m_width;
	float m_height;
	float m_rotation;
	bool m_active;
};

#endif