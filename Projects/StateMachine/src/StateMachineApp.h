#ifndef _STATE_MACHINE_APP_H_
#define _STATE_MACHINE_APP_H_

#include <BaseApp.h>

class SpriteBatch;
class Texture;
class Font;

class GameStateManager;

class StateMachineApp : public BaseApp
{
public:
	StateMachineApp() {}
	virtual ~StateMachineApp() {}

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update(float deltaTime);
	virtual void Draw();

	SpriteBatch* GetSpriteBatch() { return m_spriteBatch; }
	GameStateManager* GetGameStateManager() { return m_gameStateManager; }

protected:
	SpriteBatch* m_spriteBatch;
	GameStateManager* m_gameStateManager;
};

#endif