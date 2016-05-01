#include "ThreadingApp.h"

#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "Camera.h"
#include "Gizmos.h"
#include "FakeHardware.h"

#include <Timer.h>
#include <iostream>

using glm::vec3;
using glm::vec4;

bool ThreadingApp::Startup()
{
	if (!TestApp::Startup())
		return false;

	m_hardware = new FakeHardware(5, 10);

	return true;
}

bool ThreadingApp::Update(float deltaTime)
{
	Timer timer;
	timer.Start();

	if (glfwWindowShouldClose(m_window) || glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	m_camera->Update(deltaTime);

	Gizmos::Clear();

	int result = m_hardware->GetSomeResult();

	if (glfwGetMouseButton(m_window, 0) == GLFW_PRESS)
	{
		double x = 0;
		double y = 0;
		glfwGetCursorPos(m_window, &x, &y);

		vec4 plane(0, 1, 0, 0);
		m_pickPosition = m_camera->PickAgainstPlane((float)x, (float)y, plane);
	}

	Gizmos::AddTransform(glm::translate(m_pickPosition));

	for (int i = 0; i < 21; ++i)
	{
		Gizmos::AddLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10), i == 10 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));
		Gizmos::AddLine(vec3(10, 0, -10 + i), vec3(-10, 0, -10 + i), i == 10 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));
	}

	timer.End();

	if (timer.GetTimeDiff() > 0.5)
		std::cout << "Too slow! Frame took " << timer.GetTimeDiff() << " seconds." << std::endl;

	std::cout << "Result: " << result << std::endl;
	return true;
}

void ThreadingApp::Shutdown()
{
	delete m_hardware;

	TestApp::Shutdown();
}