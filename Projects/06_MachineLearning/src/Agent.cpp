#include "Agent.h"
#include <GLFW/glfw3.h>
#include "NeuralNetwork.h"
#include "Gizmos.h"

using glm::vec3;
using glm::vec2;

void Agent::Update(float delta)
{
	vec3 testInput = vec3(m_position.x, m_position.y, 0);
	m_clock += delta;
	m_memoryClock += delta;
	float memoryFrequency = 1.f;
	if (m_memoryClock > memoryFrequency)
	{
		m_memoryClock -= memoryFrequency;
		vec3 testInput = vec3(m_position.x, m_position.y, 0);
		AddToMemory(testInput);
		m_neuralNetwork->TrainNetwork(m_memory);
	}
	m_position += m_velocity * delta;
	if (CheckBounds())
	{
		m_facingDirection = 44.f / 7.f * (float)((rand() % 1000) / 1000.f);
		m_velocity.x = m_maxSpeed * sin(m_facingDirection);
		m_velocity.y = m_maxSpeed * cos(m_facingDirection);
	}
	m_neuralNetwork->RenderDebug(vec2(30, 30), 200, m_memory);
	m_foodClock--;
	m_waterClock--;
}

bool Agent::CheckBounds()
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &screenWidth, &screenHeight);

	if ((m_position.x < 0 && m_velocity.x < 0) || (m_position.x >screenWidth  && m_velocity.x > 0))
	{ 
		return true;
	}
	if ((m_position.y < 0 && m_velocity.y < 0) || (m_position.y >screenHeight  && m_velocity.y > 0))
	{
		return true;
	}
	return false;
}

void Agent::Setup(vec2 startPos, float size, glm::vec4 colour, float facingDirection)
{
	m_position = startPos;
	m_startingPosition = startPos;
	m_facingDirection = facingDirection;
	m_startingFacingDirection = facingDirection;
	m_diameter = size;
	m_clock = 0.f;
	m_foodClock = 0;
	m_waterClock = 0;
	m_memoryClock = 0.f;
	m_colour = colour;
	m_maxSpeed = 500.f;
	m_velocity.x = m_maxSpeed * sin(m_facingDirection);
	m_velocity.y = m_maxSpeed * cos(m_facingDirection);
	m_health1 = StartingHealth;
	m_health2 = StartingHealth;
	if (m_neuralNetwork == NULL)
	{
		int numberInputs = 2;
		int numberHiddenNeurons = 10;  //the tutorial notes claim that there are three neurons in hte hidden layer but performance is rather poor with only three and four works better. Try three as an experiment
		int numberOutputs = 4; 
		m_neuralNetwork = new NeuralNetwork(numberInputs, numberHiddenNeurons, numberOutputs);
		InitMemory(m_memory);
		m_neuralNetwork->TrainNetwork(m_memory);  //mock up for testing purposes
		m_neuralNetwork->SetUpDebugRender(30); //setup our debug 
	}
}

void Agent::InitMemory(std::vector<vec3>& memory)
{
	memory.push_back(vec3(m_position.x, m_position.y, 0));
}

void Agent::AddToMemory(vec3 newMemory)
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &screenWidth, &screenHeight);
	newMemory.x /= (float)screenWidth;
	newMemory.y /= (float)screenHeight;
	m_memory.push_back(newMemory);
}

void Agent::AddGizmo()
{
	int segments = 20;
	Gizmos::Add2DCircle(m_position, m_diameter * 1.1f / 2.f, segments, glm::vec4(0, 0, 0, 1));
	Gizmos::Add2DCircle(m_position, m_diameter * 0.9f / 2.f, segments, m_colour);
}

void Agent::HurtAgent(float damage)
{
	m_health1 -= damage;
	m_health2 -= damage;
	if (m_health1 < 0.f || m_health2 < 0.f)
	{
		int screenWidth = 0;
		int screenHeight = 0;
		glfwGetWindowSize(glfwGetCurrentContext(), &screenWidth, &screenHeight);
		AddToMemory(vec3(m_position.x, m_position.y, 1));
		m_neuralNetwork->TrainNetwork(m_memory);
		m_startingPosition.x = (float)(rand() % screenWidth);
		m_startingPosition.y = (float)(rand() % 2 * screenHeight);
		float size = 20.f;
		float facing = 44.f / 7.0f * (float)((rand() % 1000) / 1000.f);
		Setup(m_startingPosition, m_diameter, m_colour, facing);
	}
}

void Agent::FeedAgent(float foodFound)
{
	if (foodFound > 0.f)
	{
		m_health1 += foodFound;
		if (m_foodClock <= 0) //simple mechanism to stop too many food discoveries getting added to memory
		{
			AddToMemory(vec3(m_position.x, m_position.y, 2));
			m_neuralNetwork->TrainNetwork(m_memory);
			m_foodClock = 20;
		}
	}
}

void Agent::HydrateAgent(float waterFound)
{
	if (waterFound > 0.f)
	{
		m_health2 += waterFound;
		if (m_waterClock <= 0)
		{
			AddToMemory(vec3(m_position.x, m_position.y, 2));
			m_neuralNetwork->TrainNetwork(m_memory);
			m_waterClock = 20;
		}
	}
}

vec2 Agent::GetPosition()
{
	return m_position;
}