#ifndef _SPLASH_STATE_H_
#define _SPLASH_STATE_H_

#include "IGameState.h"

class SplashState : public IGameState
{
public:
	SplashState(StateMachineApp *app);
	virtual ~SplashState();

	virtual void Update(float deltaTime);
	virtual void Draw();

private:
	float m_elapsedTime;
};

#endif