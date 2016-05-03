#ifndef _GAME_STATE_MANAGER_H_
#define _GAME_STATE_MANAGER_H_

#include <map>
#include <vector>

class IGameState;

class GameStateManager
{
public:
	GameStateManager();
	~GameStateManager();

	void SetState(const char* name, IGameState* state);
	void PushState(const char* name);
	void PopState();

	void UpdateGameStates(float deltaTime);
	void DrawGameStates();

private:
	std::map<const char*, IGameState*> m_registeredStates;
	std::vector<IGameState*> m_stateStack;

	void ProcessCommands();

	enum EGameStateCommand
	{
		PUSH,
		POP,
		SET
	};

	struct GameStateCommand
	{
		EGameStateCommand opp;
		const char* stateName;
		IGameState* state;
	};

	std::vector<GameStateCommand> m_commands;
};

#endif