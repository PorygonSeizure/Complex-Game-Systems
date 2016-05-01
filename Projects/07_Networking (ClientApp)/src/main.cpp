#include "BasicNetworkingApp.h"
#include <GLFW/glfw3.h>

int main()
{
	BaseApp* app = new BasicNetworkingApp();
	if (app->Startup())
		app->Run();
	app->Shutdown();

	return 0;
}