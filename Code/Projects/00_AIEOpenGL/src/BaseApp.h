#ifndef _BASE_APP_H_
#define _BASE_APP_H_

struct GLFWwindow;

class BaseApp
{
public:
	BaseApp() {}
	virtual ~BaseApp() {}

	void Run();
	
	virtual bool Startup() = 0;
	virtual void Shutdown() = 0;

	virtual bool Update(float deltaTime) = 0;
	virtual void Draw() = 0;

protected:
	virtual bool CreateGLFWWindow(const char* title, int width, int height);
	virtual void DestroyGLFWWindow();

	GLFWwindow*	m_window;
};

#endif