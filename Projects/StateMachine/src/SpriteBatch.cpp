#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "SpriteBatch.h"
#include "Texture.h"
#include "Font.h"
#include <Shader.h>
#include <glm/ext.hpp>
#include <stb_truetype.h>

SpriteBatch::SpriteBatch()
{
	SetSpriteColor(1, 1, 1, 1);
	SetUVRect(0.f, 0.f, 1.f, 1.f);

	unsigned int pixels[1] = {0xFFFFFFFF};
	m_nullTexture = new Texture(1, 1, pixels);

	m_currentVertex = 0;
	m_currentIndex = 0;
	m_renderBegun = false;

	m_vao = -1;
	m_vbo = -1;
	m_ibo = -1;

	m_shader = new Shader();

	m_currentTexture = 0;

	for (int i = 0; i < TEXTURE_STACK_SIZE; i++)
	{
		m_textureStack[i] = nullptr;
		m_fontTexture[i] = 0;
	}

	m_shader->LoadShader(GL_VERTEX_SHADER, "../StateMachine/src/Shaders/SpriteBatchVertex.vs");
	m_shader->LoadShader(GL_FRAGMENT_SHADER, "../StateMachine/src/Shaders/SpriteBatchFragment.fs");

	//char* vertexShader = "#version 150\n \
	//					in vec4 position; \
	//					in vec4 colour; \
	//					in vec2 texcoord; \
	//					out vec4 vertexColour; \
	//					out vec2 vertexTexCoord; \
	//					out float vertexTextureID; \
	//					uniform mat4 projectionMatrix; \
	//					void main() { vertexColour = colour; vertexTexCoord = texcoord; vertexTextureID = position.w; \
	//					gl_Position = projectionMatrix * vec4(position.x, position.y, position.z, 1.f); }";
	//
	//
	//char* fragmentShader = "#version 150\n \
	//					in vec4 vertexColour; \
	//					in vec2 vertexTexCoord; \
	//					in float vertexTextureID; \
	//					out vec4 fragColour; \
	//					const int TEXTURE_STACK_SIZE = 16; \
	//					uniform sampler2D textureStack[TEXTURE_STACK_SIZE]; \
	//					uniform int fontTexture[TEXTURE_STACK_SIZE]; \
	//					void main() { int id = int(vertexTextureID); \
	//					if(id < TEXTURE_STACK_SIZE) { vec4 rgba = texture2D(textureStack[id], vertexTexCoord); \
	//						if (fontTexture[id] == 1) rgba = rgba.rrrr; fragColour = rgba * vertexColour; } \
	//					else fragColour = vertexColour; if (fragColour.a < 0.1f) discard; }";
	//
	//unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	//unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
	//
	//glShaderSource(vs, 1, (const char**)&vertexShader, 0);
	//glCompileShader(vs);
	//
	//glShaderSource(fs, 1, (const char**)&fragmentShader, 0);
	//glCompileShader(fs);

	m_shader->Link();
	m_shader->BindAttrib(0, "position");
	m_shader->BindAttrib(1, "colour");
	m_shader->BindAttrib(2, "texcoord");

	//m_shader = glCreateProgram();
	//glAttachShader(m_shader, vs);
	//glAttachShader(m_shader, fs);
	//glBindAttribLocation(m_shader, 0, "position");
	//glBindAttribLocation(m_shader, 1, "colour");
	//glBindAttribLocation(m_shader, 2, "texcoord");
	//glLinkProgram(m_shader);
	//
	//int success = GL_FALSE;
	//glGetProgramiv(m_shader, GL_LINK_STATUS, &success);
	//if (success == GL_FALSE)
	//{
	//	int infoLogLength = 0;
	//	glGetProgramiv(m_shader, GL_INFO_LOG_LENGTH, &infoLogLength);
	//	char* infoLog = new char[infoLogLength];
	//
	//	glGetProgramInfoLog(m_shader, infoLogLength, 0, infoLog);
	//	printf("Error: Failed to link SpriteBatch shader program!\n%s\n", infoLog);
	//	delete[] infoLog;
	//}

	m_shader->Bind();

	//glUseProgram(m_shader);

	// set texture locations
	char buf[32];
	for (int i = 0; i < TEXTURE_STACK_SIZE; ++i)
	{
		sprintf_s(buf, "textureStack[%i]", i);
		m_shader->GetUniform(buf);
		//glUniform1i(glGetUniformLocation(m_shader, buf), i);
	}

	glUseProgram(0);

	//glDeleteShader(vs);
	//glDeleteShader(fs);
	
	// pre calculate the indices... they will always be the same
	int index = 0;
	for (int i = 0; i < (MAX_SPRITES * 6);)
	{
		m_indices[i++] = (index + 0);
		m_indices[i++] = (index + 1);
		m_indices[i++] = (index + 2);

		m_indices[i++] = (index + 0);
		m_indices[i++] = (index + 2);
		m_indices[i++] = (index + 3);
		index += 4;
	}
	
	// create the vao, vio and vbo
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (MAX_SPRITES * 6) * sizeof(unsigned short), (void *)(&m_indices[0]), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, (MAX_SPRITES * 4) * sizeof(SBVertex), m_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(SBVertex), (char*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SBVertex), (char*)16);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SBVertex), (char*)32);
	glBindVertexArray(0);
}

SpriteBatch::~SpriteBatch()
{
	delete m_shader;
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	glDeleteBuffers(1, &m_vao);
	//glDeleteProgram(m_shader);
}

void SpriteBatch::Begin()
{
	m_renderBegun = true;
	m_currentIndex = 0;
	m_currentVertex = 0;
	m_currentTexture = 0;

	int width = 0;
	int height = 0;
	auto window = glfwGetCurrentContext();
	glfwGetWindowSize(window, &width, &height);
	
	m_shader->Bind();
	//glUseProgram(m_shader);

	auto projection = glm::ortho(0.f, (float)width, 0.f, (float)height, 1.f, -101.f);
	glUniformMatrix4fv(m_shader->GetUniform("projectionMatrix"), 1, false, &projection[0][0]);
	//glUniformMatrix4fv(glGetUniformLocation(m_shader, "projectionMatrix"), 1, false, &projection[0][0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SetSpriteColor(1, 1, 1, 1);
}

void SpriteBatch::End()
{
	if (m_renderBegun == false)
		return;

	FlushBatch();

	glUseProgram(0);

	m_renderBegun = false;
}

void SpriteBatch::DrawSprite(Texture* texture, float xPos, float yPos, float width, float height, float rotation, float depth, float xOrigin, float yOrigin)
{
	if (texture == nullptr)
		texture = m_nullTexture;

	if (ShouldFlush())
		FlushBatch();
	unsigned int textureID = PushTexture(texture);

	if (width == 0.f)
		width = (float)texture->GetWidth();
	if (height == 0.f)
		height = (float)texture->GetHeight();

	float tlX = (0.f - xOrigin) * width;
	float tlY = (0.f - yOrigin) * height;
	float trX = (1.f - xOrigin) * width;
	float trY = (0.f - yOrigin) * height;
	float brX = (1.f - xOrigin) * width;
	float brY = (1.f - yOrigin) * height;
	float blX = (0.f - xOrigin) * width;
	float blY = (1.f - yOrigin) * height;

	if (rotation != 0.f)
	{
		float si = glm::sin(rotation);
		float co = glm::cos(rotation);
		RotateAround(tlX, tlY, tlX, tlY, si, co);
		RotateAround(trX, trY, trX, trY, si, co);
		RotateAround(brX, brY, brX, brY, si, co);
		RotateAround(blX, blY, blX, blY, si, co);
	}

	int index = m_currentVertex;

	m_vertices[m_currentVertex].pos[0] = xPos + tlX;
	m_vertices[m_currentVertex].pos[1] = yPos + tlY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY + m_uvH;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = xPos + trX;
	m_vertices[m_currentVertex].pos[1] = yPos + trY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX + m_uvW;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY + m_uvH;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = xPos + brX;
	m_vertices[m_currentVertex].pos[1] = yPos + brY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX + m_uvW;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = xPos + blX;
	m_vertices[m_currentVertex].pos[1] = yPos + blY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY;
	m_currentVertex++;

	m_indices[m_currentIndex++] = (index + 0);
	m_indices[m_currentIndex++] = (index + 2);
	m_indices[m_currentIndex++] = (index + 3);

	m_indices[m_currentIndex++] = (index + 0);
	m_indices[m_currentIndex++] = (index + 1);
	m_indices[m_currentIndex++] = (index + 2);
}

void SpriteBatch::DrawSpriteTransformed3x3(Texture* texture, float* transformMat3x3, float width, float height, float depth, float xOrigin, float yOrigin)
{
	if (texture == nullptr)
		texture = m_nullTexture;

	if (ShouldFlush())
		FlushBatch();

	unsigned int textureID = PushTexture(texture);

	if (width == 0.f)
		width = (float)texture->GetWidth();
	if (height == 0.f)
		height = (float)texture->GetHeight();

	float tlX = (0.f - xOrigin) * width;
	float tlY = (0.f - yOrigin) * height;
	float trX = (1.f - xOrigin) * width;
	float trY = (0.f - yOrigin) * height;
	float brX = (1.f - xOrigin) * width;
	float brY = (1.f - yOrigin) * height;
	float blX = (0.f - xOrigin) * width;
	float blY = (1.f - yOrigin) * height;

	// transform the points by the matrix
	// 0 3 6
	// 1 4 7
	// 2 5 8
	float x;
	float y;
	x = tlX;
	y = tlY;
	tlX = x * transformMat3x3[0] + y * transformMat3x3[3] + transformMat3x3[6];
	tlY = x * transformMat3x3[1] + y * transformMat3x3[4] + transformMat3x3[7];
	x = trX;
	y = trY;
	trX = x * transformMat3x3[0] + y * transformMat3x3[3] + transformMat3x3[6];
	trY = x * transformMat3x3[1] + y * transformMat3x3[4] + transformMat3x3[7];
	x = brX;
	y = brY;
	brX = x * transformMat3x3[0] + y * transformMat3x3[3] + transformMat3x3[6];
	brY = x * transformMat3x3[1] + y * transformMat3x3[4] + transformMat3x3[7];
	x = blX;
	y = blY;
	blX = x * transformMat3x3[0] + y * transformMat3x3[3] + transformMat3x3[6];
	blY = x * transformMat3x3[1] + y * transformMat3x3[4] + transformMat3x3[7];	

	int index = m_currentVertex;
	
	m_vertices[m_currentVertex].pos[0] = tlX;
	m_vertices[m_currentVertex].pos[1] = tlY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY + m_uvH;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = trX;
	m_vertices[m_currentVertex].pos[1] = trY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX + m_uvW;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY + m_uvH;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = brX;
	m_vertices[m_currentVertex].pos[1] = brY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX + m_uvW;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = blX;
	m_vertices[m_currentVertex].pos[1] = blY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY;
	m_currentVertex++;

	m_indices[m_currentIndex++] = (index + 0);
	m_indices[m_currentIndex++] = (index + 2);
	m_indices[m_currentIndex++] = (index + 3);

	m_indices[m_currentIndex++] = (index + 0);
	m_indices[m_currentIndex++] = (index + 1);
	m_indices[m_currentIndex++] = (index + 2);
}

void SpriteBatch::DrawSpriteTransformed4x4(Texture* texture, float* transformMat4x4, float width, float height, float depth, float xOrigin, float yOrigin)
{
	if (texture == nullptr)
		texture = m_nullTexture;

	if (ShouldFlush())
		FlushBatch();
	unsigned int textureID = PushTexture(texture);

	if (width == 0.f)
		width = (float)texture->GetWidth();
	if (height == 0.f)
		height = (float)texture->GetHeight();

	float tlX = (0.f - xOrigin) * width;
	float tlY = (0.f - yOrigin) * height;
	float trX = (1.f - xOrigin) * width;
	float trY = (0.f - yOrigin) * height;
	float brX = (1.f - xOrigin) * width;
	float brY = (1.f - yOrigin) * height;
	float blX = (0.f - xOrigin) * width;
	float blY = (1.f - yOrigin) * height;

	// transform the points by the matrix
	// 0 4 8  12
	// 1 5 9  13
	// 2 6 10 14
	// 3 7 11 15
	float x;
	float y;
	x = tlX;
	y = tlY;
	tlX = x * transformMat4x4[0] + y * transformMat4x4[4] + transformMat4x4[12];
	tlY = x * transformMat4x4[1] + y * transformMat4x4[5] + transformMat4x4[13];
	x = trX;
	y = trY;
	trX = x * transformMat4x4[0] + y * transformMat4x4[4] + transformMat4x4[12];
	trY = x * transformMat4x4[1] + y * transformMat4x4[5] + transformMat4x4[13];
	x = brX;
	y = brY;
	brX = x * transformMat4x4[0] + y * transformMat4x4[4] + transformMat4x4[12];
	brY = x * transformMat4x4[1] + y * transformMat4x4[5] + transformMat4x4[13];
	x = blX;
	y = blY;
	blX = x * transformMat4x4[0] + y * transformMat4x4[4] + transformMat4x4[12];
	blY = x * transformMat4x4[1] + y * transformMat4x4[5] + transformMat4x4[13];

	int index = m_currentVertex;

	m_vertices[m_currentVertex].pos[0] = tlX;
	m_vertices[m_currentVertex].pos[1] = tlY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY + m_uvH;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = trX;
	m_vertices[m_currentVertex].pos[1] = trY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX + m_uvW;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY + m_uvH;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = brX;
	m_vertices[m_currentVertex].pos[1] = brY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX + m_uvW;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = blX;
	m_vertices[m_currentVertex].pos[1] = blY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY;
	m_currentVertex++;
	
	m_indices[m_currentIndex++] = (index + 0);
	m_indices[m_currentIndex++] = (index + 2);
	m_indices[m_currentIndex++] = (index + 3);

	m_indices[m_currentIndex++] = (index + 0);
	m_indices[m_currentIndex++] = (index + 1);
	m_indices[m_currentIndex++] = (index + 2);
}

void SpriteBatch::DrawLine(float x1, float y1, float x2, float y2, float thickness, float depth)
{
	float xDiff = x2 - x1;
	float yDiff = y2 - y1;
	float len = glm::sqrt(xDiff * xDiff + yDiff * yDiff);
	float xDir = xDiff / len;
	float yDir = yDiff / len;

	float rot = glm::atan(yDir, xDir);

	float uvX = m_uvX;
	float uvY = m_uvY;
	float uvW = m_uvW;
	float uvH = m_uvH;

	SetUVRect(0.f, 0.f, 1.f, 1.f);

	DrawSprite(m_nullTexture, x1, y1, len, thickness, rot, depth, 0.f, 0.5f);

	SetUVRect(uvX, uvY, uvW, uvH);
}

void SpriteBatch::DrawSpriteBatchText(Font* font, const char* text, float xPos, float yPos, float depth)
{
	if (font == nullptr ||
		font->m_glHandle == 0)
		return;

	stbtt_aligned_quad q;

	if (ShouldFlush() || m_currentTexture >= TEXTURE_STACK_SIZE - 1)
		FlushBatch();

	glActiveTexture(GL_TEXTURE0 + m_currentTexture++);
	glBindTexture(GL_TEXTURE_2D, font->GetHandle());
	glActiveTexture(GL_TEXTURE0);
	m_fontTexture[m_currentTexture - 1] = 1;

	// font renders top to bottom, so we need to invert it
	int w = 0;
	int h = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &w, &h);

	yPos = h - yPos;

	while (*text)
	{
		if (ShouldFlush() || m_currentTexture >= TEXTURE_STACK_SIZE - 1)
		{
			FlushBatch();

			glActiveTexture(GL_TEXTURE0 + m_currentTexture++);
			glBindTexture(GL_TEXTURE_2D, font->GetHandle());
			glActiveTexture(GL_TEXTURE0);
			m_fontTexture[m_currentTexture - 1] = 1;
		}

		stbtt_GetBakedQuad((stbtt_bakedchar*)font->m_glyphData, font->m_sizeOfBytesX, font->m_sizeOfBytesY, (unsigned char)*text, &xPos, &yPos, &q, 1);

		int index = m_currentVertex;

		m_vertices[m_currentVertex].pos[0] = q.x0;
		m_vertices[m_currentVertex].pos[1] = h - q.y1;
		m_vertices[m_currentVertex].pos[2] = depth;
		m_vertices[m_currentVertex].pos[3] = (float)m_currentTexture - 1;
		m_vertices[m_currentVertex].color[0] = m_r;
		m_vertices[m_currentVertex].color[1] = m_g;
		m_vertices[m_currentVertex].color[2] = m_b;
		m_vertices[m_currentVertex].color[3] = m_a;
		m_vertices[m_currentVertex].texcoord[0] = q.s0;
		m_vertices[m_currentVertex].texcoord[1] = q.t1;
		m_currentVertex++;
		m_vertices[m_currentVertex].pos[0] = q.x1;
		m_vertices[m_currentVertex].pos[1] = h - q.y1;
		m_vertices[m_currentVertex].pos[2] = depth;
		m_vertices[m_currentVertex].pos[3] = (float)m_currentTexture - 1;
		m_vertices[m_currentVertex].color[0] = m_r;
		m_vertices[m_currentVertex].color[1] = m_g;
		m_vertices[m_currentVertex].color[2] = m_b;
		m_vertices[m_currentVertex].color[3] = m_a;
		m_vertices[m_currentVertex].texcoord[0] = q.s1;
		m_vertices[m_currentVertex].texcoord[1] = q.t1;
		m_currentVertex++;
		m_vertices[m_currentVertex].pos[0] = q.x1;
		m_vertices[m_currentVertex].pos[1] = h - q.y0;
		m_vertices[m_currentVertex].pos[2] = depth;
		m_vertices[m_currentVertex].pos[3] = (float)m_currentTexture - 1;
		m_vertices[m_currentVertex].color[0] = m_r;
		m_vertices[m_currentVertex].color[1] = m_g;
		m_vertices[m_currentVertex].color[2] = m_b;
		m_vertices[m_currentVertex].color[3] = m_a;
		m_vertices[m_currentVertex].texcoord[0] = q.s1;
		m_vertices[m_currentVertex].texcoord[1] = q.t0;
		m_currentVertex++;
		m_vertices[m_currentVertex].pos[0] = q.x0;
		m_vertices[m_currentVertex].pos[1] = h - q.y0;
		m_vertices[m_currentVertex].pos[2] = depth;
		m_vertices[m_currentVertex].pos[3] = (float)m_currentTexture - 1;
		m_vertices[m_currentVertex].color[0] = m_r;
		m_vertices[m_currentVertex].color[1] = m_g;
		m_vertices[m_currentVertex].color[2] = m_b;
		m_vertices[m_currentVertex].color[3] = m_a;
		m_vertices[m_currentVertex].texcoord[0] = q.s0;
		m_vertices[m_currentVertex].texcoord[1] = q.t0;
		m_currentVertex++;
		
		m_indices[m_currentIndex++] = (index + 0);
		m_indices[m_currentIndex++] = (index + 2);
		m_indices[m_currentIndex++] = (index + 3);

		m_indices[m_currentIndex++] = (index + 0);
		m_indices[m_currentIndex++] = (index + 1);
		m_indices[m_currentIndex++] = (index + 2);

		text++;
	}
}

void SpriteBatch::FlushBatch()
{
	//dont render anything
	if (m_currentVertex == 0 || m_currentIndex == 0 || m_renderBegun == false)
		return;
	char buf[32];

	for (int i = 0; i < TEXTURE_STACK_SIZE; ++i)
	{
		sprintf_s(buf, "fontTexture[%i]", i);
		glUniform1i(m_shader->GetUniform(buf), m_fontTexture[i]);
		//glUniform1i(glGetUniformLocation(m_shader, buf), m_fontTexture[i]);
	}

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

	glBufferSubData(GL_ARRAY_BUFFER, 0, m_currentVertex * sizeof(SBVertex), m_vertices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_currentIndex * sizeof(unsigned short), m_indices);

	glDrawElements(GL_TRIANGLES, m_currentIndex, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	// clear the active textures
	for (unsigned int i = 0; i < m_currentTexture; i++)
	{
		m_textureStack[i] = nullptr;
		m_fontTexture[i] = 0;
	}

	// reset vertex, index and texture count
	m_currentIndex = 0;
	m_currentVertex = 0;
	m_currentTexture = 0;
}

unsigned int SpriteBatch::PushTexture(Texture* texture)
{
	// check if the texture is already in use
	// if so, return as we dont need to add it to our list of active txtures again
	for (unsigned int i = 0; i <= m_currentTexture; i++)
	{
		if (m_textureStack[i] == texture)
			return i;
	}

	// if we've used all the textures we can, than we need to flush to make room for another texture change
	if (m_currentTexture >= TEXTURE_STACK_SIZE - 1)
		FlushBatch();

	// add the texture to our active texture list
	m_textureStack[m_currentTexture] = texture;

	glActiveTexture(GL_TEXTURE0 + m_currentTexture);
	glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
	glActiveTexture(GL_TEXTURE0);

	// return what the current texture was and increment
	return m_currentTexture++;
}

void SpriteBatch::SetSpriteColor(float r, float g, float b, float a)
{
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}

void SpriteBatch::SetUVRect(float uvX, float uvY, float uvW, float uvH)
{
	m_uvX = uvX;
	m_uvY = uvY;
	m_uvW = uvW;
	m_uvH = uvH;
}

void SpriteBatch::RotateAround(float inX, float inY, float& outX, float& outY, float sin, float cos)
{
	outX = inX * cos - inY * sin;
	outY = inX * sin + inY * cos;
}