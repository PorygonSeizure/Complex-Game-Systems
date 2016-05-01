
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "GeneticApp.h"
#include <GLFW/glfw3.h>

int main()
{
	BaseApp* app = new GeneticApp();
	if (app->Startup())
		app->Run();
	app->Shutdown();

	delete app;

	_CrtDumpMemoryLeaks();

	return 0;
}