#include "AI.h"
#include <GLFW/glfw3.h>
#include "Gizmos.h"
#include "../GraphNode.h"
#include "../Graph.h"
#include "Walls.h"

#include <Windows.h>

using glm::vec4;
using glm::vec2;

void AI::Update(float deltaTime)
{
	m_clock += deltaTime;
	m_memoryClock += deltaTime;
	float memoryFrequency = 1.f;
	if (m_memoryClock > memoryFrequency)
	{
		m_memoryClock -= memoryFrequency;
		Memory testInput(m_startingPosition, m_startingNode, m_position, m_node, m_clock);
		AddToMemory(testInput);
		//m_neuralNetwork->TrainNetwork(m_oldMemory);
	}
	//m_position += m_velocity * delta;
	//if (CheckBounds())
	//{
	//	m_facingDirection = 44.f / 7.f * (float)((rand() % 1000) / 1000.f);
	//	m_velocity.x = m_maxSpeed * sin(m_facingDirection);
	//	m_velocity.y = m_maxSpeed * cos(m_facingDirection);
	//}
	//m_neuralNetwork->RenderDebug(vec2(30, 30), 200, m_oldMemory);
	m_foodClock--;
}

bool AI::CheckBounds()
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &screenWidth, &screenHeight);

	if ((m_position.x < 0 && m_velocity.x < 0) || (m_position.x > screenWidth  && m_velocity.x > 0))
	{
		return true;
	}
	if ((m_position.y < 0 && m_velocity.y < 0) || (m_position.y > screenHeight  && m_velocity.y > 0))
	{
		return true;
	}
	return false;
}

void AI::Setup(GraphNode* startNode, float size, vec4 colour, float facingDirection)
{
	Evaluate();
	m_position = startNode->GetPos();
	m_startingPosition = startNode->GetPos();
	m_node = startNode;
	m_startingNode = startNode;
	m_facingDirection = facingDirection;
	m_startingFacingDirection = facingDirection;
	m_diameter = size;
	m_clock = 0.f;
	m_foodClock = 0;
	m_memoryClock = 0.f;
	m_colour = colour;
	m_maxSpeed = 500.f;
	m_score = 0;
	m_velocity.x = m_maxSpeed * sin(m_facingDirection);
	m_velocity.y = m_maxSpeed * cos(m_facingDirection);
	//if (m_neuralNetwork == NULL)
	//{
	//	int numberInputs = 2;
	//	int numberHiddenNeurons = 10;
	//	int numberOutputs = 4;
	//	m_neuralNetwork = new NeuralNetwork(numberInputs, numberHiddenNeurons, numberOutputs);
	//	InitMemory(m_oldMemory);
	//	m_neuralNetwork->TrainNetwork(m_oldMemory);  //mock up for testing purposes
	//	m_neuralNetwork->SetupDebugRender(30); //setup our debug 
	//}
}

void AI::InitMemory(std::vector<Memory>& memory)
{
	memory.push_back(Memory(m_startingPosition, m_startingNode, m_position, m_node, m_clock));
}

void AI::AddToMemory(Memory newMemory)
{
	int screenWidth = 0;
	int screenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &screenWidth, &screenHeight);
	newMemory.endPosition.x /= (float)screenWidth;
	newMemory.endPosition.y /= (float)screenHeight;

	m_memory.push_back(newMemory);
}

void AI::AddGizmo()
{
	int segments = 20;
	Gizmos::Add2DCircle(m_position, m_diameter * 1.1f / 2.f, segments, glm::vec4(0, 0, 0.5, 1));
	Gizmos::Add2DCircle(m_position, m_diameter * 0.9f / 2.f, segments, m_colour);
}

void AI::HurtAI(bool contact)
{
	if (contact)
	{
		AddToMemory(Memory(m_startingPosition, m_startingNode, m_position, m_node, m_clock));
		//m_neuralNetwork->TrainNetwork(m_oldMemory);
		float size = 20.f;
		float facing = 44.f / 7.0f * (float)((rand() % 1000) / 1000.f);
		Setup(m_startingNode, m_diameter, m_colour, facing);
	}
}

void AI::FeedAI(bool contact)
{
	if (contact)
	{
		if (m_foodClock <= 0) //simple mechanism to stop too many food discoveries getting added to memory
		{
			m_score++;
			AddToMemory(Memory(m_startingPosition, m_startingNode, m_position, m_node, m_clock));
			//m_neuralNetwork->TrainNetwork(m_oldMemory);
			m_foodClock = 20;
		}
	}
}

void AI::Flip(int i)
{
	(m_genome[i] == 0) ? (m_genome[i] = 0) : (m_genome[i] = 1);
}

void AI::SetBit(bool bit, int i)
{
	m_genome[i] = bit;
}

vec2 AI::GetPosition()
{
	return m_position;
}

vec2 AI::GetVelocity()
{
	return m_velocity;
}

GraphNode* AI::GetNode()
{
	return m_node;
}

int AI::GetFitness()
{
	return m_fitness;
}

std::bitset<256> AI::GetBitset()
{
	return m_genome;
}

void AI::Evaluate()
{
	float temp = (float)m_score;
	temp *= (m_clock / 1000.f);
	vec2 difference = m_position - m_startingPosition;
	if (difference.x < 0)
		difference.x = -difference.x;
	if (difference.y < 0)
		difference.y = -difference.y;
	temp *= ((difference.x + difference.y) / 10.f);

	m_fitness = (int)temp;
}

void AI::DijkstraAStarMovement(std::vector<GraphNode*>& path)
{
	if (!path.empty())
	{
		m_node = *path.begin();
		m_position = m_node->GetPos();
		path.erase(path.begin());
	}
}

void AI::BitsetMovement(Graph* graph/*, Wall* walls*/)
{
	for (unsigned int i = 0; i < m_genome.size(); i = i + 2)
	{
		if (m_genome[i] == 0)
		{
			if (m_genome[i + 1] == 0 && m_node->GetLoc().x > 1)
			{
				m_node = graph->GetNode(m_node->GetLoc().x - 1, m_node->GetLoc().y);
			}
			else if (m_genome[i + 1] == 1 && m_node->GetLoc().x < 62)
			{
				m_node = graph->GetNode(m_node->GetLoc().x + 1, m_node->GetLoc().y);
			}
		}
		else if (m_genome[i] == 1)
		{
			if (m_genome[i + 1] == 0 && m_node->GetLoc().x < 1)
			{
				m_node = graph->GetNode(m_node->GetLoc().x, m_node->GetLoc().y - 1);
			}
			else if (m_genome[i + 1] == 1 && m_node->GetLoc().y < 62)
			{
				m_node = graph->GetNode(m_node->GetLoc().x, m_node->GetLoc().y + 1);
			}
		}

		m_position = m_node->GetPos();

		Sleep(1000);
	}
}