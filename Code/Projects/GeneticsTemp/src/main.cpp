#include "GeneticApp.h"
#include <GLFW/glfw3.h>

int main()
{
	BaseApp* app = new GeneticApp();
	if (app->Startup())
		app->Run();
	app->Shutdown();

	delete app;

	return 0;
}