#ifndef _GENETICAL_ALGORITHMS_H_
#define _GENETICAL_ALGORITHMS_H_

#include <bitset>
#include <vector>

struct Entity
{
	Entity()
	{
		for (unsigned int i = 0; i < genome.size(); ++i)
			genome[i] = rand() % 1000 < 500;
	}
	void Flip(int i)
	{
		(genome[i] == 0) ? (genome[i] = 0) : (genome[i] = 1);
	}
	std::bitset<256> genome; // 128 moves

	int fitness;
};

class Genetics
{
public:
	Genetics() : m_generation(0), m_fittest(INT_MAX) {}
	~Genetics() {}

	void Evaluate(Entity& entity);
	Entity* RouletteWheelSelection(std::vector<Entity>);
	void MainLoop();

private:
	int m_generation;
	int m_fittest;

	bool m_maze[32][32];

	std::vector<Entity> m_population;
};

#endif