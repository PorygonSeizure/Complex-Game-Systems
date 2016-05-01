#ifndef _GENETICAL_ALGORITHMS_H_
#define _GENETICAL_ALGORITHMS_H_

#include <bitset>
#include <vector>
#include "glm/glm.hpp"

class AI;

class Genetics
{
public:
	Genetics() : m_generation(0), m_fittest(INT_MAX) {}
	~Genetics() {}

	void Evaluate(AI* entity);
	AI* RouletteWheelSelection(std::vector<AI*>);
	void MainLoop();

private:
	int m_generation;
	int m_fittest;

	bool m_maze[32][32];

	std::vector<AI*> m_population;
};

#endif