#include "stdafx.h"
#include "Game.h"
#include "Exception.h"

void Dar::Game::Run()
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		Initialize();
		m_isRunning = true;

		while (m_isRunning)
		{
			InvokeOnUpdate();
			InvokeOnDraw();
		}
	}
	catch (Exception ex)
	{
		MessageBox(m_window.GetHandle(), ex.message.c_str(), L"Fatal error", MB_ABORTRETRYIGNORE);
		throw;	
	}

	Shutdown(); //TODO: call Shutdown before throw or not?
}

void Dar::Game::Exit()
{
	m_isRunning = false;
}

Dar::Game::Game(HINSTANCE instanceHandle, const std::wstring& windowTitle, int showCommand)
	:m_instanceHandle {instanceHandle}, m_window{instanceHandle, windowTitle, showCommand}
{
}

void Dar::Game::Initialize()
{
	m_window.Show();
	m_clock.Reset();
}

void Dar::Game::Shutdown()
{
	//TODO: implement
}

void Dar::Game::InvokeOnUpdate() const
{
	//TODO: implement
}

void Dar::Game::InvokeOnDraw() const
{
	//TODO: implement
}
