#ifndef _UTILITY_SYSTEM_APP_
#define _UTILITY_SYSTEM_APP_

#include "BaseApp.h"

// only needed for the camera picking
#include <glm/vec3.hpp>

class Camera;
class World;
class BaseNPC;

class UtilitySystemApp: public BaseApp
{
public:
	UtilitySystemApp() : m_camera(nullptr) {}
	virtual ~UtilitySystemApp() {}

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update(float deltaTime);
	virtual void Draw();

private:

	Camera* m_camera;

	World* m_world;
	BaseNPC* m_NPC;
};

#endif