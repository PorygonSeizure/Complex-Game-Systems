#pragma once

#include "glm/glm.hpp"
#include <vector>

const int StartingHealth = 1;

class NeuralNetwork;

class Agent
{
	float _memoryClock;
	NeuralNetwork* neuralNetwork = NULL;
	glm::vec2 _position;
	float _facingDirection;
	glm::vec2 _startingPosition;
	float _startingFacingDirection;

	float _diameter;
	float _clock;
	int _foodClock;
	glm::vec2 _velocity;

	bool _active;
	glm::vec4 _colour;
	float _maxSpeed;
	bool checkBounds();
	void fakeMemory(std::vector<glm::vec3>&);
	void initMemory(std::vector<glm::vec3>&);
	float health;
	std::vector<glm::vec3> memory;
	void addToMemory(glm::vec3);
public:
	Agent();
	~Agent();
	void setup(glm::vec2 startPos, float size, glm::vec4 colour,float facingDirection);
	void update(float delta);
	void draw();
	void addGizmo();
	void hurtAgent(float damage);
	void feedAgent(float food);
	glm::vec2 getPosition();
};