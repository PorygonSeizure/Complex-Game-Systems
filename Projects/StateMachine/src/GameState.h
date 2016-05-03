#ifndef _GAME_STATE_H_
#define _GAME_STATE_H_

#include "IGameState.h"

class GameState : public IGameState
{
public:
	GameState(StateMachineApp *app);
	virtual ~GameState();

	virtual void Update(float deltaTime);
	virtual void Draw();
};

#endif