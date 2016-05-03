#ifndef _FONT_H_
#define _FONT_H_

class Font
{
	friend class SpriteBatch;

public:
	Font(const char* trueTypeFontFile, unsigned short fontHeight);
	~Font();

	unsigned int GetHandle() const { return m_glHandle; }

private:
	void* m_glyphData;
	unsigned int m_glHandle;
	unsigned int m_pixelBufferHandle;
	unsigned short m_sizeOfBytesX;
	unsigned short m_sizeOfBytesY;
};

#endif