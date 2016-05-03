#ifndef _I_GAME_STATE_H_
#define _I_GAME_STATE_H_

class StateMachineApp;

class IGameState
{
public:
	IGameState(StateMachineApp *app) : m_app(app) {}
	virtual ~IGameState() {}

	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;

	StateMachineApp* GetApp() { return m_app; }

private:
	StateMachineApp* m_app;
};

#endif