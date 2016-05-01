#ifndef _TEST_APP_H_
#define _TEST_APP_H_

#include "BaseApp.h"

//only needed for the camera picking
#include <glm/vec3.hpp>

class Camera;

class TestApp : public BaseApp
{
public:
	TestApp() : m_camera(nullptr) {}
	virtual ~TestApp() {};

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update(float deltaTime);
	virtual void Draw();

protected:
	Camera* m_camera;

	//this is an example position for camera picking
	glm::vec3 m_pickPosition;
};

#endif