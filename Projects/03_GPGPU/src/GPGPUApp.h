#ifndef _GPGPU_APP_H_
#define _GPGPU_APP_H_

#include <BaseApp.h>
#include <glm/glm.hpp>
#include <CL/cl.h>

class Camera;

class GPGPUApp : public BaseApp
{
public:
	GPGPUApp() {};
	virtual ~GPGPUApp() {};

	virtual bool Startup();
	virtual bool Update(float deltaTime);
	virtual void Draw();
	virtual void Shutdown();

private:
	Camera* m_camera;
	glm::vec3 m_pickPosition;

	static const int VECTOR_COUNT = 5000000;
	glm::vec4 m_vectors[VECTOR_COUNT];

	cl_platform_id m_platform;
	cl_device_id m_device;

	cl_context m_context;

	cl_command_queue m_queue;

	cl_program m_program;

	cl_kernel m_kernel;

	cl_mem m_buffer;
};

#endif