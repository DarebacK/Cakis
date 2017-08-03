#include "stdafx.h"
#include "Game.h"

void DarEngine::Game::Run()
{
	Initialize();
	InvokeOnInitialization();

	while(m_isRunning)
	{
		InvokeOnUpdate();
		InvokeOnDraw();
	}
	
	Shutdown();
}

void DarEngine::Game::Exit()
{
	m_isRunning = false;
}

DarEngine::Game::Game(HINSTANCE instanceHandle, const std::wstring& windowClassName, const std::wstring& windowTitle, int showCommand)
	:m_instanceHandle(instanceHandle), m_windowClassName(windowClassName), m_windowTitle(windowTitle), m_showCommand(showCommand)
{
}

void DarEngine::Game::Initialize()
{
	m_isRunning = true;
}

void DarEngine::Game::Shutdown()
{
	//TODO: implement
}

void DarEngine::Game::InvokeOnUpdate() const
{
	//TODO: implement
}

void DarEngine::Game::InvokeOnDraw() const
{
	//TODO: implement
}

void DarEngine::Game::InvokeOnInitialization() const
{
	//TODO: implement
}

void DarEngine::Game::InvokeOnExit() const
{
	//TODO: implement
}
