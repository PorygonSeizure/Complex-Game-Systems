#include "GPGPUApp.h"
#include <GLFW/glfw3.h>

int main()
{
	GPGPUApp* app = new GPGPUApp();

	if (app->Startup())
		app->Run();
	app->Shutdown();

	delete app;
	return 0;
}