#ifndef _SPRITE_BATCH_H_
#define _SPRITE_BATCH_H_

class Texture;
class Font;
class Shader;

class SpriteBatch
{
public:
	SpriteBatch();
	virtual ~SpriteBatch();

	// all draw calls must occur between a begin / end pair
	virtual void Begin();
	virtual void End();

	// if texture is null then it renders a coloured sprite
	// depth is in the range [0,100] with lower being closer
	virtual void DrawSprite(Texture* texture, float xPos, float yPos, float width = 0.f, float height = 0.f, float rotation = 0.f, float depth = 0.f, float xOrigin = 0.5f, float yOrigin = 0.5f);
	virtual void DrawSpriteTransformed3x3(Texture* texture, float* transformMat3x3, float width = 0.f, float height = 0.f, float depth = 0.f, float xOrigin = 0.5f, float yOrigin = 0.5f);
	virtual void DrawSpriteTransformed4x4(Texture* texture, float* transformMat4x4, float width = 0.f, float height = 0.f, float depth = 0.f, float xOrigin = 0.5f, float yOrigin = 0.5f);

	// draws a simple coloured line with a given thickness
	// depth is in the range [0,100] with lower being closer
	virtual void DrawLine(float x1, float y1, float x2, float y2, float thickness = 1.f, float depth = 0.f);

	// draws simple text on the screen horizontally
	// depth is in the range [0,100] with lower being closer
	virtual void DrawSpriteBatchText(Font* font, const char* text, float xPos, float yPos, float depth = 0.f);

	// sets the colour of sprites for all subsequent draw calls
	void SetSpriteColor(float r, float g, float b, float a);

	// can be used to set the texture coordinates of sprites using textures
	// for all subsequent drawSprite calls
	void SetUVRect(float uvX, float uvY, float uvW, float uvH);

protected:
	// helper methods used during drawing
	bool ShouldFlush() { return m_currentVertex >= 2048; }
	void FlushBatch();
	unsigned int PushTexture(Texture* texture);

	// indicates in the middle of a begin/end pair
	bool m_renderBegun;

	// texture handling
	enum { TEXTURE_STACK_SIZE = 16 };
	Texture* m_nullTexture;
	Texture* m_textureStack[TEXTURE_STACK_SIZE];
	int m_fontTexture[TEXTURE_STACK_SIZE];
	unsigned int m_currentTexture;

	// texture coordinate information
	float m_uvX;
	float m_uvY;
	float m_uvW;
	float m_uvH;

	// sprite colour
	float m_r;
	float m_g;
	float m_b;
	float m_a;

	// sprite handling
	enum { MAX_SPRITES = 512 };
	struct SBVertex
	{
		float pos[4];
		float color[4];
		float texcoord[2];
	};

	SBVertex m_vertices[MAX_SPRITES * 4];
	unsigned short m_indices[MAX_SPRITES * 6];
	int m_currentVertex;
	int m_currentIndex;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	// shader used to render sprites
	//unsigned int m_shader;
	Shader* m_shader;

	// helper method used to rotate sprites around a pivot
	void RotateAround(float inX, float inY, float& outX, float& outY, float sin, float cos);
};

#endif