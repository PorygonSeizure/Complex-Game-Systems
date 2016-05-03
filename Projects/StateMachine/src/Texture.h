#ifndef _TEXTURE_H_
#define _TEXTURE_H_

class Texture
{
public:
	Texture(const char* filename);
	Texture(unsigned int width, unsigned int height, unsigned int* pixels = 0);
	virtual ~Texture();

	unsigned int GetWidth() const { return m_width; }
	unsigned int GetHeight() const { return m_height; }

	unsigned int GetHandle() const { return m_handle; }

protected:
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_handle;
};

#endif