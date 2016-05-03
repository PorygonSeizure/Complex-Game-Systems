#include "GameState.h"
#include "StateMachineApp.h"
#include "SpriteBatch.h"

GameState::GameState(StateMachineApp *app) : IGameState(app)
{

}

GameState::~GameState()
{

}

void GameState::Update(float deltaTime)
{

}

void GameState::Draw()
{
	GetApp()->GetSpriteBatch()->Begin();
	GetApp()->GetSpriteBatch()->DrawLine(10.f, 10.f, 620.f, 10.f);
	GetApp()->GetSpriteBatch()->End();
}