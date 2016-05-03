#include "StateMachineApp.h"

int main()
{	
	BaseApp* app = new StateMachineApp();
	if (app->Startup())
		app->Run();
	app->Shutdown();

	return 0;
}