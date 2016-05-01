#include "Node.h"
#include <iostream>
#include <Timer.h>
#include <thread>
#include <vector>

using std::thread;

void AddChildren1(Node* root, int childrenPerNode, int depth)
{
	if (depth > 0)
	{
		for (int i = 0; i < childrenPerNode; i++)
		{
			Node* child = new Node();
			AddChildren1(child, childrenPerNode, depth - 1);
			root->AddChild(child);
		}
	}
}

void AddChildren2(Node* root, int childrenPerNode, int depth)
{
	if (depth > 0)
	{
		Node* child = new Node[childrenPerNode];
		root->data = child;

		for (int i = 0; i < childrenPerNode; i++)
		{
			AddChildren2(&child[i], childrenPerNode, depth - 1);
			root->AddChild(&child[i]);
		}
	}
}

void SubMain1()
{
	double time = 0.0;
	Node* root = new Node();
	AddChildren1(root, 5, 6);
	for (int i = 0; i < 5; i++)
	{
		time += root->Loop();
	}
	time = time / 5;
	delete root;
	std::cout << "Graph 1 took " << time << " seconds." << std::endl;
}

void SubMain2()
{
	double time = 0.0;
	Node* root = new Node();
	AddChildren2(root, 5, 6);
	for (int i = 0; i < 5; i++)
	{
		time += root->Loop();
	}
	time = time / 5;
	delete root;
	std::cout << "Graph 2 took " << time << " seconds." << std::endl;
}

void main()
{
	std::vector<thread> threads;

	threads.push_back(thread(SubMain1));
	threads.push_back(thread(SubMain2));

	for (auto& thread : threads)
		thread.join();

	system("PAUSE");
}