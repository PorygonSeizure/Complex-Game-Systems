#include "gl_core_4_4.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const char * filename) : m_width(0), m_height(0), m_handle(0)
{
	int x = 0;
	int y = 0;
	int comp = 0;
	unsigned char* data = stbi_load(filename, &x, &y, &comp, STBI_default);
	
	if (data != nullptr)
	{
		glGenTextures(1, &m_handle);
		glBindTexture(GL_TEXTURE_2D, m_handle);
		switch (comp) {
		case STBI_grey:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y,
						 0, GL_RED, GL_UNSIGNED_BYTE, data);
			break;
		case STBI_grey_alpha:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, x, y,
						 0, GL_RG, GL_UNSIGNED_BYTE, data);
			break;
		case STBI_rgb:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y,
						 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			break;
		case STBI_rgb_alpha:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y,
						 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			break;
		default:	break;
		};
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);
		m_width = (unsigned int)x;
		m_height = (unsigned int)y;
	}
}

Texture::Texture(unsigned int width, unsigned int height, unsigned int * pixels) : m_width(width), m_height(height)
{
	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glBindTexture( GL_TEXTURE_2D, 0 );
}

Texture::~Texture()
{
	if (m_handle != 0)
		glDeleteTextures(1, &m_handle);
}