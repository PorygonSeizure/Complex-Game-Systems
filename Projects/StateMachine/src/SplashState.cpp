#include "SplashState.h"
#include "StateMachineApp.h"
#include "SpriteBatch.h"
#include "GameStateManager.h"

SplashState::SplashState(StateMachineApp *app) : IGameState(app)
{
	m_elapsedTime = 0.f;
}

SplashState::~SplashState()
{

}

void SplashState::Update(float deltaTime)
{
	m_elapsedTime += deltaTime;
	if (m_elapsedTime > 3.f)
	{
		GetApp()->GetGameStateManager()->PopState();
		GetApp()->GetGameStateManager()->PushState("GameState");
	}
}

void SplashState::Draw()
{
	GetApp()->GetSpriteBatch()->Begin();
	GetApp()->GetSpriteBatch()->DrawLine(10.f, 10.f, 620.f, 10.f);
	GetApp()->GetSpriteBatch()->End();
}