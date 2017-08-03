#include "stdafx.h"
#include "Game.h"
#include "Exception.h"

void DarEngine::Game::Run()
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		Initialize();

		while (m_isRunning)
		{
			InvokeOnUpdate();
			InvokeOnDraw();
		}
	}
	catch (Exception ex)
	{
		MessageBox(m_windowHandle, ex.message.c_str(), L"Fatal error", MB_ABORTRETRYIGNORE);
		throw;	
	}

	Shutdown(); //TODO: call Shutdown before throw or not?
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



	InvokeOnInitialization();
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

LRESULT DarEngine::Game::WndProc(HWND windowHandle, UINT message, WPARAM, LPARAM lParam)
{
}
