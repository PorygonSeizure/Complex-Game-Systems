#include "GPGPUApp.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <Timer.h>
#include "Gizmos.h"
#include "Camera.h"
#include <thread>
#include <vector>

using std::cout;
using std::endl;
using glm::vec4;
using glm::vec3;
using std::thread;

bool GPGPUApp::Startup()
{
	//create a basic window
	CreateGLFWWindow("AIE OpenGL Application", 1280, 720);

	//start the gizmo system that can draw basic shapes
	Gizmos::Create();

	//create a camera
	m_camera = new Camera(glm::pi<float>() * 0.25f, 16.f / 9.f, 0.1f, 1000.f);
	m_camera->SetLookAtFrom(vec3(10, 10, 10), vec3(0));

	for (auto& v : m_vectors)
		v = vec4(1.f);

	//get the first platform
	cl_int result = clGetPlatformIDs(1, &m_platform, nullptr);
	if (result != CL_SUCCESS)
		cout << "clGetPlatformIDs failed: " << result << endl;

	//get the first device on the platform (default is GPU)
	result = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_DEFAULT, 1, &m_device, nullptr);
	if (result != CL_SUCCESS)
		cout << "clGetDeviceIDs failed: " << result << endl;

	//create a context for a device on the specified platform
	cl_context_properties contextProperties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)m_platform, 0 };
	m_context = clCreateContext(contextProperties, 1, &m_device, nullptr, nullptr, &result);
	if (result != CL_SUCCESS)
		cout << "clCreateContext failed: " << result << endl;

	//create a command queue to process commands
	m_queue = clCreateCommandQueue(m_context, m_device, CL_QUEUE_PROFILING_ENABLE, &result);
	if (result != CL_SUCCESS)
		cout << "clCreateCommandQueue failed: " << result << endl;

	FILE* file = nullptr;
	fopen_s(&file, "../03_GPGPU/src/kernelFile.cl", "rb");
	if (file == nullptr)
		return false;
	//read the program source
	fseek(file, (long)0, SEEK_END);
	unsigned int kernelSize = ftell(file);
	fseek(file, (long)0, SEEK_SET);
	char* kernelSource = new char[kernelSize + 1];
	memset(kernelSource, 0, kernelSize + 1);
	fread(kernelSource, sizeof(char), kernelSize, file);
	fclose(file);

	//build program from our source using the device context
	m_program = clCreateProgramWithSource(m_context, 1, (const char**)&kernelSource, &kernelSize, &result);
	if (result != CL_SUCCESS)
		cout << "clCreateProgramWithSource failed: " << result << endl;

	result = clBuildProgram(m_program, 1, &m_device, nullptr, nullptr, nullptr);
	if (result != CL_SUCCESS)
		cout << "clBuildProgram failed: " << result << endl;

	//extract the kernel
	m_kernel = clCreateKernel(m_program, "normalize_vec4", &result);
	if (result != CL_SUCCESS)
		cout << "clCreateKernel failed: " << result << endl;

	//create cl buffer for our data and copy it off the host (CPU)
	m_buffer = clCreateBuffer(m_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(vec4) * VECTOR_COUNT, m_vectors, &result);
	if (result != CL_SUCCESS)
		cout << "clCreateBuffer failed: " << result << endl;

	//set the buffer as the first argument for our kernel
	result = clSetKernelArg(m_kernel, 0, sizeof(cl_mem), &m_buffer);
	if (result != CL_SUCCESS)
		cout << "clSetKernelArg failed: " << result << endl;

	m_pickPosition = vec3(0.f
		);

	return true;
}

void Update1(vec4* vectors, const int size)
{
	Timer timer;
	timer.Start();
	for (int i = 0; i < size; i++)
		vectors[i] = glm::normalize(vectors[i]);
	timer.End();
	cout << "CPU duration: " << timer.GetTimeDiff() << endl;
}

void Update2(const unsigned int size, cl_command_queue queue, cl_kernel kernel, cl_mem buffer, vec4* vectors)
{
	cl_int* error = 0;

	//process the kernel and give it an event ID
	cl_event eventID = 0;

	//we'll give it a size equal to the number of vec4's to process
	size_t globalWorkSize[] = { size };

	cl_int result = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, globalWorkSize, nullptr, 0, nullptr, &eventID);
	if (result != CL_SUCCESS)
		cout << "clEnqueueNDRangeKernel failed: " << result << endl;

	//read back the processed data but wait for an event to complete
	result = clEnqueueReadBuffer(queue, buffer, CL_TRUE, 0, sizeof(vec4) * size, vectors, 1, &eventID, nullptr);
	if (result != CL_SUCCESS)
		cout << "clEnqueueReadBuffer failed: " << result << endl;

	//finish all opencl commands
	clFlush(queue);
	clFinish(queue);

	//how long did each event take?
	//get start/end profile data for the event
	cl_ulong clStartTime = 0;
	result = clGetEventProfilingInfo(eventID, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &clStartTime, nullptr);

	if (result != CL_SUCCESS)
		cout << "clGetEventProfilingInfo failed: " << result << endl;

	cl_ulong clEndTime = 0;
	result = clGetEventProfilingInfo(eventID, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &clEndTime, nullptr);

	if (result != CL_SUCCESS)
		cout << "clGetEventProfilingInfo failed: " << result << endl;

	//return time is in nanoseconds, so convert to seconds
	double clTime = (clEndTime - clStartTime) * 1.0e-9;
	cout << "GPU duration: " << clTime << endl;

	clEnqueueMapBuffer(queue, buffer, CL_TRUE, CL_MAP_WRITE, 0, sizeof(vec4) * size, 1, &eventID, nullptr, error);
	if (error != CL_SUCCESS)
		cout << error << endl;
}

void Update3(GLFWwindow* window, vec3 pickPosition, Camera* camera)
{
	Timer timer;
	timer.Start();
	if (glfwGetMouseButton(window, 0) == GLFW_PRESS)
	{
		double x = 0;
		double y = 0;
		glfwGetCursorPos(window, &x, &y);

		vec4 plane(0, 1, 0, 0);
		pickPosition = camera->PickAgainstPlane((float)x, (float)y, plane);
	}
	Gizmos::AddTransform(glm::translate(pickPosition));

	for (int i = 0; i < 21; ++i)
	{
		Gizmos::AddLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10), i == 10 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));

		Gizmos::AddLine(vec3(10, 0, -10 + i), vec3(-10, 0, -10 + i), i == 10 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));
	}
	timer.End();
	cout << "Gizmos duration: " << timer.GetTimeDiff() << endl;
}

bool GPGPUApp::Update(float deltaTime)
{
	if (glfwWindowShouldClose(m_window) || glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	m_camera->Update(deltaTime);

	Gizmos::Clear();

	std::vector<thread> threads;

	threads.push_back(thread(Update1, m_vectors, VECTOR_COUNT));
	threads.push_back(thread(Update2, VECTOR_COUNT, m_queue, m_kernel, m_buffer, m_vectors));
	threads.push_back(thread(Update3, m_window, m_pickPosition, m_camera));
	
	for (auto& thread : threads)
		thread.join();

	






	

	return true;
}

void GPGPUApp::Shutdown()
{
	clReleaseContext(m_context);
	clReleaseCommandQueue(m_queue);
	clReleaseProgram(m_program);
	clReleaseKernel(m_kernel);
	clReleaseMemObject(m_buffer);

	delete m_camera;
	Gizmos::Destroy();

	DestroyGLFWWindow();
}

void GPGPUApp::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	Gizmos::Draw(m_camera->GetProjectionView());

	int width = 0;
	int height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	glm::mat4 guiMatrix = glm::ortho<float>(0.f, (float)width, 0.f, (float)height);

	Gizmos::Draw2D(guiMatrix);
}