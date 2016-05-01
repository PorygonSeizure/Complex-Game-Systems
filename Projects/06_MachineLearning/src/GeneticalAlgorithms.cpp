#include "GeneticalAlgorithms.h"
// 00 left
// 01 right
// 10 up
// 11 down

/*struct GAEntity {
	GAEntity() {
		for (int i = 0 i < genome.size() ; ++i)
			genome[i] = rand() % 1000 < 500;
	}
	std::bitset<256> genome; // 128 moves
	
	int fitness;
};*/

//bool maze[32][32];

//std::vector<GAEntity> population(100);

void Genetics::Evaluate(Entity& entity)
{
	//calculate fitness, like do a maze walk
	//entity.fitness = ???;
}

Entity* Genetics::RouletteWheelSelection(std::vector<Entity> population)
{
	int totalFitness = 0;
	for (auto& e : population)
		totalFitness += e.fitness;

	int selection = rand() % totalFitness;
	totalFitness = 0;

	for (auto& e : population)
	{
		totalFitness += e.fitness;
		if (selection < totalFitness)
			return &e;
	}

	return nullptr;
}

void Genetics::MainLoop()
{
	while (true)
	{
		for (auto& e : m_population)
		{
			Evaluate(e);
			int score = e.fitness;
			if (score < m_fittest)
				m_fittest = score;
		}

		if (m_fittest == 0)
			break;

		++m_generation;

		std::vector<Entity> nextGeneration;

		while (m_population.size() != nextGeneration.size())
		{
			auto iterator = RouletteWheelSelection(m_population);
			Entity parent1 = *iterator;

			iterator = RouletteWheelSelection(m_population);
			Entity parent2 = *iterator;

			Entity sibling1;
			Entity sibling2;

			//cross-over
			if (rand() % 1000 < 700)
			{
				//if using bitset
				for (int i = 0; i < 256; ++i)
					sibling1.genome[i] = i < 128 ? parent1.genome[i] : parent2.genome[i];

				for (int i = 0; i < 256; ++i)
					sibling2.genome[i] = i < 128 ? parent2.genome[i] : parent1.genome[i];
			}
			else
			{
				sibling1 = parent1;
				sibling2 = parent2;
			}

			//mutate
			for (unsigned int i = 0; i < sibling1.genome.size(); ++i)
				if (rand() % 1000 < 5)
					sibling1.Flip(i);
			for (unsigned int i = 0; i < sibling2.genome.size(); ++i)
				if (rand() % 1000 < 5)
					sibling2.Flip(i);

			//populate new generation
			nextGeneration.push_back(sibling1);
			nextGeneration.push_back(sibling2);
		}

		m_population = nextGeneration;
	}
}