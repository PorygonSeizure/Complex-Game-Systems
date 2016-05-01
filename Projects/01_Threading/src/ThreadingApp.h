#ifndef _THREADING_APP_H_
#define _THREADING_APP_H_

#include <TestApp.h>

class FakeHardware;

class ThreadingApp : public TestApp
{
public:
	ThreadingApp() {}
	~ThreadingApp() {}

	bool Startup();
	void Shutdown();

	bool Update(float deltaTime);

protected:
	FakeHardware* m_hardware;
};

#endif