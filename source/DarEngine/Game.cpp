#include "stdafx.h"
#include "Game.h"
#include "Exception.h"

void Dar::Game::Run()
{
	try
	{
		Initialize();
		RunGameLoop();
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
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

void Dar::Game::Initialize()
{
	m_window.Show();
	m_clock.Reset();
}

void Dar::Game::RunGameLoop()
{
	m_isRunning = true;

	MSG message;
	ZeroMemory(&message, sizeof(message));

	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			m_clock.Update();
			auto deltaTime = ComputeDeltaTime(m_clock);

			//TODO: update and draw
		}
	}
}

void Dar::Game::Shutdown()
{
	//TODO: implement
}
