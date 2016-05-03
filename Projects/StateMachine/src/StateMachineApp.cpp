#include "StateMachineApp.h"
#include <GLFW/glfw3.h>

#include "SpriteBatch.h"
#include "SplashState.h"
#include "GameState.h"
#include "GameStateManager.h"

bool StateMachineApp::Startup()
{	
	CreateGLFWWindow("Game State Manager", 640, 480);

	m_spriteBatch = new SpriteBatch();
	//m_state = new SplashState(this);
	//m_state = new GameState(this);
	m_gameStateManager = new GameStateManager();

	m_gameStateManager->SetState("SplashState", new SplashState(this));
	//m_gameStateManager->SetState("GameState", new GameState(this));

	m_gameStateManager->PushState("GameState");

	return true;
}

void StateMachineApp::Shutdown()
{
	delete m_gameStateManager;
	delete m_spriteBatch;

	DestroyGLFWWindow();
}

bool StateMachineApp::Update(float deltaTime)
{	
	if (glfwWindowShouldClose(m_window) || glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	m_gameStateManager->UpdateGameStates(deltaTime);

	return true;
}

void StateMachineApp::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_gameStateManager->DrawGameStates();
}