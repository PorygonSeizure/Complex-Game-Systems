//ImGui GLFW binding with OpenGL3 + shaders
//You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
//If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
//If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
//https://github.com/ocornut/imgui

#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

//GL_CORE/GLFW
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"

#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

//Data
static GLFWwindow* window = NULL;
static double time = 0.0f;
static bool mousePressed[3] = { false, false, false };
static float mouseWheel = 0.0f;
static GLuint fontTexture = 0;
static int shaderHandle = 0;
static int vertHandle = 0;
static int fragHandle = 0;
static int attribLocationTex = 0;
static int attribLocationProjMtx = 0;
static int attribLocationPosition = 0;
static int attribLocationUV = 0;
static int attribLocationColor = 0;
static unsigned int vboHandle = 0;
static unsigned int vaoHandle = 0;
static unsigned int elementsHandle = 0;

//This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
//If text or lines are blurry when integrating ImGui in your engine:
//- in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
void ImGuiImplGlfwGL3RenderDrawLists(ImDrawData* drawData)
{
	//Backup GL state
	GLint lastProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);
	GLint lastTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
	GLint lastArrayBuffer;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &lastArrayBuffer);
	GLint lastElementArrayBuffer;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &lastElementArrayBuffer);
	GLint lastVertexArray;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &lastVertexArray);
	GLint lastBlendSrc;
	glGetIntegerv(GL_BLEND_SRC, &lastBlendSrc);
	GLint lastBlendDst;
	glGetIntegerv(GL_BLEND_DST, &lastBlendDst);
	GLint lastBlendEquationRgb;
	glGetIntegerv(GL_BLEND_EQUATION_RGB, &lastBlendEquationRgb);
	GLint lastBlendEquationAlpha;
	glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &lastBlendEquationAlpha);
	GLint lastViewport[4];
	glGetIntegerv(GL_VIEWPORT, lastViewport);
	GLboolean lastEnableBlend = glIsEnabled(GL_BLEND);
	GLboolean lastEnableCullFace = glIsEnabled(GL_CULL_FACE);
	GLboolean lastEnableDepthTest = glIsEnabled(GL_DEPTH_TEST);
	GLboolean lastEnableScissorTest = glIsEnabled(GL_SCISSOR_TEST);

	//Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	//Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
	ImGuiIO& io = ImGui::GetIO();
	int fbWidth = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fbHeight = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fbWidth == 0 || fbHeight == 0)
		return;
	drawData->ScaleClipRects(io.DisplayFramebufferScale);

	//Setup viewport, orthographic projection matrix
	glViewport(0, 0, (GLsizei)fbWidth, (GLsizei)fbHeight);
	const float orthoProjection[4][4] =
	{
		{ 2.0f / io.DisplaySize.x, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
		{ 0.0f, 0.0f, -1.0f, 0.0f },
		{ -1.0f, 1.0f, 0.0f, 1.0f },
	};
	glUseProgram(shaderHandle);
	glUniform1i(attribLocationTex, 0);
	glUniformMatrix4fv(attribLocationProjMtx, 1, GL_FALSE, &orthoProjection[0][0]);
	glBindVertexArray(vaoHandle);

	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmdList = drawData->CmdLists[n];
		const ImDrawIdx* idxBufferOffset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmdList->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmdList->VtxBuffer.front(), GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmdList->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmdList->IdxBuffer.front(), GL_STREAM_DRAW);

		for (const ImDrawCmd* pcmd = cmdList->CmdBuffer.begin(); pcmd != cmdList->CmdBuffer.end(); pcmd++)
		{
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmdList, pcmd);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(fbHeight - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idxBufferOffset);
			}
			idxBufferOffset += pcmd->ElemCount;
		}
	}

	//Restore modified GL state
	glUseProgram(lastProgram);
	glBindTexture(GL_TEXTURE_2D, lastTexture);
	glBindVertexArray(lastVertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, lastArrayBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lastElementArrayBuffer);
	glBlendEquationSeparate(lastBlendEquationRgb, lastBlendEquationAlpha);
	glBlendFunc(lastBlendSrc, lastBlendDst);
	if (lastEnableBlend)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
	if (lastEnableCullFace)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	if (lastEnableDepthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	if (lastEnableScissorTest)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);
	glViewport(lastViewport[0], lastViewport[1], (GLsizei)lastViewport[2], (GLsizei)lastViewport[3]);
}

static const char* ImGuiImplGlfwGL3GetClipboardText()
{
	return glfwGetClipboardString(window);
}

static void ImGuiImplGlfwGL3SetClipboardText(const char* text)
{
	glfwSetClipboardString(window, text);
}

void ImGuiImplGlfwGL3MouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/)
{
	if (action == GLFW_PRESS && button >= 0 && button < 3)
		mousePressed[button] = true;
}

void ImGuiImplGlfwGL3ScrollCallback(GLFWwindow*, double /*xOffset*/, double yOffset)
{
	mouseWheel += (float)yOffset;	//Use fractional mouse wheel, 1.0 unit 5 lines.
}

void ImGuiImplGlfwGL3KeyCallback(GLFWwindow*, int key, int, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (action == GLFW_PRESS)
		io.KeysDown[key] = true;
	if (action == GLFW_RELEASE)
		io.KeysDown[key] = false;

	(void)mods;	//Modifiers are not reliable across systems
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
}

void ImGuiImplGlfwGL3CharCallback(GLFWwindow*, unsigned int c)
{
	ImGuiIO& io = ImGui::GetIO();
	if (c > 0 && c < 0x10000)
		io.AddInputCharacter((unsigned short)c);
}

bool ImGuiImplGlfwGL3CreateFontsTexture()
{
	//Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width;
	int height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);	//Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

	//Upload texture to graphics system
	GLint lastTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
	glGenTextures(1, &fontTexture);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	//Store our identifier
	io.Fonts->TexID = (void*)(intptr_t)fontTexture;

	//Restore state
	glBindTexture(GL_TEXTURE_2D, lastTexture);

	return true;
}

bool ImGuiImplGlfwGL3CreateDeviceObjects()
{
	//Backup GL state
	GLint lastTexture;
	GLint lastArrayBuffer;
	GLint lastVertexArray;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &lastArrayBuffer);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &lastVertexArray);

	const GLchar* vertexShader =
		"#version 330\n"
		"uniform mat4 projMtx;\n"
		"in vec2 position;\n"
		"in vec2 UV;\n"
		"in vec4 color;\n"
		"out vec2 fragUV;\n"
		"out vec4 fragColor;\n"
		"void main()\n"
		"{\n"
		"	fragUV = UV;\n"
		"	fragColor = color;\n"
		"	gl_Position = projMtx * vec4(position.xy, 0, 1);\n"
		"}\n";

	const GLchar* fragmentShader =
		"#version 330\n"
		"uniform sampler2D texture;\n"
		"in vec2 fragUV;\n"
		"in vec4 fragColor;\n"
		"out vec4 outColor;\n"
		"void main()\n"
		"{\n"
		"	outColor = fragColor * texture(texture, fragUV.st);\n"
		"}\n";

	shaderHandle = glCreateProgram();
	vertHandle = glCreateShader(GL_VERTEX_SHADER);
	fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertHandle, 1, &vertexShader, 0);
	glShaderSource(fragHandle, 1, &fragmentShader, 0);
	glCompileShader(vertHandle);
	glCompileShader(fragHandle);
	glAttachShader(shaderHandle, vertHandle);
	glAttachShader(shaderHandle, fragHandle);
	glLinkProgram(shaderHandle);

	attribLocationTex = glGetUniformLocation(shaderHandle, "texture");
	attribLocationProjMtx = glGetUniformLocation(shaderHandle, "projMtx");
	attribLocationPosition = glGetAttribLocation(shaderHandle, "position");
	attribLocationUV = glGetAttribLocation(shaderHandle, "UV");
	attribLocationColor = glGetAttribLocation(shaderHandle, "color");

	glGenBuffers(1, &vboHandle);
	glGenBuffers(1, &elementsHandle);

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glEnableVertexAttribArray(attribLocationPosition);
	glEnableVertexAttribArray(attribLocationUV);
	glEnableVertexAttribArray(attribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
	glVertexAttribPointer(attribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(attribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(attribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

	ImGuiImplGlfwGL3CreateFontsTexture();

	//Restore modified GL state
	glBindTexture(GL_TEXTURE_2D, lastTexture);
	glBindBuffer(GL_ARRAY_BUFFER, lastArrayBuffer);
	glBindVertexArray(lastVertexArray);

	return true;
}

void ImGuiImplGlfwGL3InvalidateDeviceObjects()
{
	if (vaoHandle)
		glDeleteVertexArrays(1, &vaoHandle);
	if (vboHandle)
		glDeleteBuffers(1, &vboHandle);
	if (elementsHandle)
		glDeleteBuffers(1, &elementsHandle);
	vaoHandle = vboHandle = elementsHandle = 0;

	glDetachShader(shaderHandle, vertHandle);
	glDeleteShader(vertHandle);
	vertHandle = 0;

	glDetachShader(shaderHandle, fragHandle);
	glDeleteShader(fragHandle);
	fragHandle = 0;

	glDeleteProgram(shaderHandle);
	shaderHandle = 0;

	if (fontTexture)
	{
		glDeleteTextures(1, &fontTexture);
		ImGui::GetIO().Fonts->TexID = 0;
		fontTexture = 0;
	}
}

bool ImGuiImplGlfwGL3Init(GLFWwindow* window, bool installCallbacks)
{
	window = window;

	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;	//Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	io.RenderDrawListsFn = ImGuiImplGlfwGL3RenderDrawLists;	//Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
	io.SetClipboardTextFn = ImGuiImplGlfwGL3SetClipboardText;
	io.GetClipboardTextFn = ImGuiImplGlfwGL3GetClipboardText;
#ifdef _WIN32
	io.ImeWindowHandle = glfwGetWin32Window(window);
#endif

	if (installCallbacks)
	{
		glfwSetMouseButtonCallback(window, ImGuiImplGlfwGL3MouseButtonCallback);
		glfwSetScrollCallback(window, ImGuiImplGlfwGL3ScrollCallback);
		glfwSetKeyCallback(window, ImGuiImplGlfwGL3KeyCallback);
		glfwSetCharCallback(window, ImGuiImplGlfwGL3CharCallback);
	}

	return true;
}

void ImGuiImplGlfwGL3Shutdown()
{
	ImGuiImplGlfwGL3InvalidateDeviceObjects();
	ImGui::Shutdown();
}

void ImGuiImplGlfwGL3NewFrame()
{
	if (!fontTexture)
		ImGuiImplGlfwGL3CreateDeviceObjects();

	ImGuiIO& io = ImGui::GetIO();

	//Setup display size (every frame to accommodate for window resizing)
	int w;
	int h;
	int displayW;
	int displayH;
	glfwGetWindowSize(window, &w, &h);
	glfwGetFramebufferSize(window, &displayW, &displayH);
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)displayW / w) : 0, h > 0 ? ((float)displayH / h) : 0);

	//Setup time step
	double currentTime = glfwGetTime();
	io.DeltaTime = time > 0.0 ? (float)(currentTime - time) : (float)(1.0f / 60.0f);
	time = currentTime;

	//Setup inputs
	//(we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
	if (glfwGetWindowAttrib(window, GLFW_FOCUSED))
	{
		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		io.MousePos = ImVec2((float)mouseX, (float)mouseY);	//Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
	}
	else
		io.MousePos = ImVec2(-1, -1);

	for (int i = 0; i < 3; i++)
	{
		io.MouseDown[i] = mousePressed[i] || glfwGetMouseButton(window, i) != 0;	//If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
		mousePressed[i] = false;
	}

	io.MouseWheel = mouseWheel;
	mouseWheel = 0.0f;

	//Hide OS mouse cursor if ImGui is drawing it
	glfwSetInputMode(window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

	//Start the frame
	ImGui::NewFrame();
}