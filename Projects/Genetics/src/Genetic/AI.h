#ifndef _AI_H_
#define _AI_H_

#include "glm/glm.hpp"
#include <vector>
#include <bitset>

class GraphNode;
class Graph;
class Wall;

struct Memory
{
	Memory(glm::vec2 sp, GraphNode* sn, glm::vec2 ep, GraphNode* en, float t) : startPosition(sp), startNode(sn), endPosition(ep), endNode(en), timeAlive(t) {}
	~Memory() {}

	glm::vec2 startPosition;
	GraphNode* startNode;
	glm::vec2 endPosition;
	GraphNode* endNode;
	float timeAlive;
};

class AI
{
public:
	AI()
	{
		for (unsigned int i = 0; i < m_genome.size(); ++i)
			m_genome[i] = rand() % 1000 < 500;
	};
	~AI() {};

	void Setup(GraphNode* startNode, float size, glm::vec4 colour, float facingDirection);
	void Update(float deltaTime);
	void Draw() {};
	void AddGizmo();
	void HurtAI(bool contact);
	void FeedAI(bool contact);
	void Flip(int i);
	void SetBit(bool bit, int i);
	void Evaluate();

	glm::vec2 GetPosition();
	glm::vec2 GetVelocity();

	GraphNode* GetNode();

	int GetFitness();

	std::bitset<256> GetBitset();

	void BitsetMovement(Graph* graph/*, Wall* walls*/);

private:
	float m_memoryClock;
	float m_facingDirection;
	float m_startingFacingDirection;
	float m_diameter;
	float m_clock;
	float m_maxSpeed;

	GraphNode* m_node;
	GraphNode* m_startingNode;

	glm::vec2 m_position;
	glm::vec2 m_startingPosition;
	glm::vec2 m_velocity;

	int m_foodClock;
	int m_score;
	int m_fitness;

	bool m_active;

	glm::vec4 m_colour;

	std::vector<Memory> m_memory;
	std::bitset<256> m_genome;

	bool CheckBounds();

	void FakeMemory(std::vector<Memory>&);
	void InitMemory(std::vector<Memory>&);
	void AddToMemory(Memory newMemory);

	void DijkstraAStarMovement(std::vector<GraphNode*>& path);
};

#endif