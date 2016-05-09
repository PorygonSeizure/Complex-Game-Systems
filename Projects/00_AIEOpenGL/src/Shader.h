#ifndef _SHADER_H_
#define _SHADER_H_

class Shader
{
public:
	Shader();
	~Shader();

	bool LoadShader(unsigned int type, const char* filename);
	bool CreateShader(unsigned int type, const char* string);
	bool Link();

	void Bind();
	void BindAttrib(unsigned int location, const char* attribName);
	void TransformVaryings(int size, const char* varyings[]);

	int GetUniform(const char* name);

	unsigned int GetHandle() const { return m_program; }

	const char* GetLastError() const { return m_error; }

private:
	unsigned int m_program;
	// vert, cont, eval, geom, frag
	unsigned int m_shaders[5];
	char* m_error;
};

#endif