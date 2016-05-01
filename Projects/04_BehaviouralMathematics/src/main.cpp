#include "UtilitySystemApp.h"
#include <GLFW/glfw3.h>

int main() {
	
	BaseApp* app = new UtilitySystemApp();
	if (app->Startup())
		app->Run();
	app->Shutdown();

	return 0;
}