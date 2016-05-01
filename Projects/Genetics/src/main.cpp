#include "GeneticsApp.h"
#include <GLFW/glfw3.h>

int main()
{
	BaseApp* app = new GeneticsApp();
	if (app->Startup())
		app->Run();
	app->Shutdown();

	delete app;
	return 0;
}