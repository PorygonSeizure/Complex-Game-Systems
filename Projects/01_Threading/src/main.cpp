#include <thread>
//#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include "ThreadingApp.h"

using std::thread;

void Alt(ThreadingApp* app)
{
	if (app->Startup())
		app->Run();
	app->Shutdown();
}

int main()
{
	std::vector<thread> threads;
	glm::vec4 vectors[50000] = {};
	int chunkLength = 50000 / 10;

	ThreadingApp* app = new ThreadingApp();

	threads.push_back(thread(Alt, app));

	for (int i = 0; i < 10; i++)
	{
		threads.push_back(thread([&](int low, int high)
		{
			for (int j = low; j < high; j++)
			{
				vectors[j] = glm::normalize(vectors[j]);
				//std::cout << "Hello Thread " << j << " " << vectors[j].x << " " << vectors[j].y << " " << vectors[j].z << " " << vectors[j].w << std::endl;
			}
		}, i * chunkLength, (i + 1) * chunkLength
		));
	}

	for (auto& thread : threads)
		thread.join();

	return 0;
}