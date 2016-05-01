#ifndef _AGENT_H_
#define _AGENT_H_

#include "glm/glm.hpp"
#include <vector>

const int StartingHealth = 1;

class NeuralNetwork;

class Agent
{
public:
	Agent() {};
	~Agent() {};

	void Setup(glm::vec2 startPos, float size, glm::vec4 colour, float facingDirection);
	void Update(float delta);
	void Draw() {};
	void AddGizmo();
	void HurtAgent(float damage);
	void FeedAgent(float food);
	void HydrateAgent(float water);

	glm::vec2 GetPosition();

private:
	float m_memoryClock;
	float m_facingDirection;
	float m_startingFacingDirection;
	float m_diameter;
	float m_clock;
	float m_maxSpeed;
	float m_health1;
	float m_health2;

	NeuralNetwork* m_neuralNetwork = NULL;

	glm::vec2 m_position;
	glm::vec2 m_startingPosition;
	glm::vec2 m_velocity;

	int m_foodClock;
	int m_waterClock;

	bool m_active;

	glm::vec4 m_colour;

	std::vector<glm::vec3> m_memory;
	
	bool CheckBounds();

	void FakeMemory(std::vector<glm::vec3>&);
	void InitMemory(std::vector<glm::vec3>&);
	void AddToMemory(glm::vec3);
};

#endif