#include "GameStateManager.h"
#include "IGameState.h"

GameStateManager::GameStateManager()
{

}

GameStateManager::~GameStateManager()
{
	for (auto iter = m_registeredStates.begin(); iter != m_registeredStates.end(); iter++)
	{
		IGameState* state = iter->second;
		delete state;
	}
	m_registeredStates.clear();
}

void GameStateManager::SetState(const char* name, IGameState* state)
{
	auto iter = m_registeredStates.find(name);
	if (iter != m_registeredStates.end())
		delete iter->second;
	m_registeredStates[name] = state;
}

void GameStateManager::PushState(const char* name)
{
	GameStateCommand cmd;
	cmd.opp = PUSH;
	cmd.stateName = name;

	m_commands.push_back(cmd);
}

void GameStateManager::PopState()
{
	GameStateCommand cmd;
	cmd.opp = POP;
	cmd.stateName = nullptr;

	m_commands.push_back(cmd);
}

void GameStateManager::UpdateGameStates(float deltaTime)
{
	ProcessCommands();

	for (auto iter = m_stateStack.begin(); iter != m_stateStack.end(); iter++)
	{
		(*iter)->Update(deltaTime);
	}
}

void GameStateManager::DrawGameStates()
{
	for (auto iter = m_stateStack.begin(); iter != m_stateStack.end(); iter++)
	{
		(*iter)->Draw();
	}
}

void GameStateManager::ProcessCommands()
{
	for (auto iter = m_commands.begin(); iter != m_commands.end(); iter++)
	{
		EGameStateCommand cmd = (*iter).opp;
		IGameState* state = (*iter).state;
		const char* name = (*iter).stateName;

		if (cmd == PUSH)
		{
			auto iter = m_registeredStates.find(name);
			if (iter != m_registeredStates.end())
			{
				//TODO: check if state is already pushed on to the state stack... if so, don't push it on again
			
				if (iter->second != nullptr)
					m_stateStack.push_back(iter->second);
			}
		}
		else if (cmd == POP)
		{
			if (m_stateStack.size() > 0)
				m_stateStack.pop_back();
		}
		else if (cmd == SET)
		{
			auto iter = m_registeredStates.find(name);
			if (iter != m_registeredStates.end())
				delete iter->second;
			m_registeredStates[name] = state;
		}
	}

	m_commands.clear();
}