#include "Shader.h"
#include "gl_core_4_4.h"
#include <stdio.h>
#include <iostream>

Shader::Shader() : m_program(0), m_error(nullptr)
{
	m_shaders[0] = m_shaders[1] = m_shaders[2] = m_shaders[3] = m_shaders[4] = 0;
	m_error = new char[5];
}

Shader::~Shader()
{
	delete[] m_error;
	
	glDeleteProgram(m_program);
	for (auto& s : m_shaders)
		glDeleteShader(s);
}

bool Shader::LoadShader(unsigned int type, const char* filename)
{
	unsigned int shader = glCreateShader(type);

	//open file
	FILE* file = nullptr;
	fopen_s(&file, filename, "rb");
	fseek(file, 0, SEEK_END);
	unsigned int size = ftell(file);
	char* source = new char[size + 1];
	fseek(file, 0, SEEK_SET);
	fread(source, sizeof(char), size, file);
	fclose(file);
	source[size] = 0;

	glShaderSource(shader, 1, (const char**)&source, 0);
	glCompileShader(shader);

	delete[] source;

	int success = GL_TRUE;
	glGetShaderiv(shader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		delete[] m_error;
		m_error = new char[infoLogLength];
		glGetShaderInfoLog(shader, infoLogLength, 0, m_error);
		return false;
	}

	switch (type)
	{
	case GL_VERTEX_SHADER:
		m_shaders[0] = shader;
		break;
	case GL_TESS_CONTROL_SHADER:
		m_shaders[1] = shader;
		break;
	case GL_TESS_EVALUATION_SHADER:
		m_shaders[2] = shader;
		break;
	case GL_GEOMETRY_SHADER:
		m_shaders[3] = shader;
		break;
	case GL_FRAGMENT_SHADER:
		m_shaders[4] = shader;
		break;
	default: std::cout << "INVALID SHADER TYPE LOADED!" << std::endl;
	}
	return true;
}

bool Shader::CreateShader(unsigned int type, const char* string)
{
	unsigned int shader = glCreateShader(type);

	glShaderSource(shader, 1, (const char**)&string, 0);
	glCompileShader(shader);
	
	int success = GL_TRUE;
	glGetShaderiv(shader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		delete[] m_error;
		m_error = new char[infoLogLength];
		glGetShaderInfoLog(shader, infoLogLength, 0, m_error);
		return false;
	}

	switch (type)
	{
	case GL_VERTEX_SHADER:
		m_shaders[0] = shader;
		break;
	case GL_TESS_CONTROL_SHADER:
		m_shaders[1] = shader;
		break;
	case GL_TESS_EVALUATION_SHADER:
		m_shaders[2] = shader;
		break;
	case GL_GEOMETRY_SHADER:
		m_shaders[3] = shader;
		break;
	case GL_FRAGMENT_SHADER:
		m_shaders[4] = shader;
		break;
	default: std::cout << "INVALID SHADER TYPE LOADED!" << std::endl;;
	}
	return true;
}

bool Shader::Link()
{
	m_program = glCreateProgram();
	for (auto& s : m_shaders)
		glAttachShader(m_program, s);
	glLinkProgram(m_program);

	int success = GL_TRUE;
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLength);

		delete[] m_error;
		m_error = new char[infoLogLength];
		glGetProgramInfoLog(m_program, infoLogLength, 0, m_error);
		return false;
	}
	return true;
}

void Shader::Bind() { glUseProgram(m_program); }

void Shader::BindAttrib(unsigned int location, const char* attribName) { glBindAttribLocation(m_program, location, attribName); }

int Shader::GetUniform(const char* name) { return glGetUniformLocation(m_program, name); }

void Shader::TransformVaryings(int size, const char* varyings[]) { glTransformFeedbackVaryings(m_program, size, varyings, GL_INTERLEAVED_ATTRIBS); }