#include "BaseApp.h"
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

bool BaseApp::CreateGLFWWindow(const char* title, int width, int height)
{
	if (glfwInit() == GL_FALSE)
		return false;

	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (m_window == nullptr)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(m_window, [](GLFWwindow*, int w, int h){ glViewport(0, 0, w, h); });

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	std::cout << "GL: " << major << "." << minor << std::endl;

	glClearColor(0.25f, 0.25f, 0.25f, 1.f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	return true;
}

void BaseApp::DestroyGLFWWindow()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void BaseApp::Run()
{
	double prevTime = glfwGetTime();
	double currTime = 0.0;

	while (true)
	{
		currTime = glfwGetTime();
		float deltaTime = (float)(currTime - prevTime);
		prevTime = currTime;

		//if the program has been stoped for logner than 12 FPS
		//cap dt to 12 FPS
		if (deltaTime > 1.f / 12.f)
			deltaTime = 1.f / 12.f;

		if(!Update(deltaTime))
			break;

		glfwPollEvents();
		Draw();
		glfwSwapBuffers(m_window);
	}
}