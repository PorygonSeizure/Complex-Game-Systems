#include "GeneticalAlgorithms.h"
#include "Genetic/AI.h"
// 00 left
// 01 right
// 10 up
// 11 down

//bool maze[32][32];

//std::vector<GAEntity> population(100);

using glm::vec4;

void Genetics::Evaluate(AI* entity)
{
	//calculate fitness, like do a maze walk
	//entity.fitness = ???;
}

AI* Genetics::RouletteWheelSelection(std::vector<AI*> population)
{
	int totalFitness = 0;
	for (auto& e : population)
		totalFitness += e->GetFitness();

	int selection = rand() % totalFitness;
	totalFitness = 0;

	for (auto& e : population)
	{
		totalFitness += e->GetFitness();
		if (selection < totalFitness)
			return e;
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
			int score = e->GetFitness();
			if (score < m_fittest)
				m_fittest = score;
		}

		if (m_fittest == 0)
			break;

		++m_generation;

		std::vector<AI*> nextGeneration;

		while (m_population.size() != nextGeneration.size())
		{
			auto iterator = RouletteWheelSelection(m_population);
			AI* parent1 = iterator;

			iterator = RouletteWheelSelection(m_population);
			AI* parent2 = iterator;

			AI* sibling1;
			AI* sibling2;

			//cross-over
			if (rand() % 1000 < 700)
			{
				//if using bitset
				for (int i = 0; i < 256; ++i)
					sibling1->GetBitset()[i] = i < 128 ? parent1->GetBitset()[i] : parent2->GetBitset()[i];

				for (int i = 0; i < 256; ++i)
					sibling2->GetBitset()[i] = i < 128 ? parent2->GetBitset()[i] : parent1->GetBitset()[i];
			}
			else
			{
				sibling1 = parent1;
				sibling2 = parent2;
			}

			//mutate
			for (unsigned int i = 0; i < sibling1->GetBitset().size(); ++i)
			{
				if (rand() % 1000 < 5)
					sibling1->Flip(i);
			}
			for (unsigned int i = 0; i < sibling2->GetBitset().size(); ++i)
			{
				if (rand() % 1000 < 5)
					sibling2->Flip(i);
			}

			//populate new generation
			nextGeneration.push_back(sibling1);
			nextGeneration.push_back(sibling2);
		}

		m_population = nextGeneration;
	}
}

void Genetics::MainLoop()
{
	while (true)
	{
		AI* parent1;
		AI* parent2;

		for (auto& e : m_population)
		{
			if (parent1->GetFitness() <= e->GetFitness())
			{
				parent2 = parent1;
				parent1 = e;
			}
			else
			{
				if (parent2->GetFitness() < e->GetFitness())
					parent2 = e;
			}
		}

		std::vector<AI*> nextGeneration;

		while (m_population.size() != nextGeneration.size())
		{
			AI* sibling;

			sibling1.behaviour = (parent1.behaviour + parent2.behaviour) / 2;

			//populate new generation
			nextGeneration.push_back(sibling);
		}

		m_population = nextGeneration;
	}
}