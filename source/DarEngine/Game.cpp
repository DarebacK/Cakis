#include "stdafx.h"
#include "Game.h"
#include "Diagnostics/Exception.h"

void DE::Game::Run()
{
	try
	{
		Initialize();
		RunGameLoop();
	}
	catch (Diagnostics::Exception ex)
	{
		MessageBox(m_window.GetHandle(), ex.message.c_str(), L"Fatal error", MB_OK);
		throw;
	}
	catch (std::exception ex)
	{
		MessageBoxA(m_window.GetHandle(), ex.what(), "Fatal error", MB_OK);
	}
}

void DE::Game::Exit()
{
	PostQuitMessage(0);
}

DE::Game::Game(HINSTANCE instanceHandle, const std::wstring& windowTitle, int showCommand)
	:m_instanceHandle {instanceHandle}, m_window{instanceHandle, windowTitle, showCommand}, 
	m_d3dContext{ m_window.GetHandle(), m_window.GetClientAreaWidth(), m_window.GetClientAreaHeight() }
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

void DE::Game::Initialize()
{
	m_window.Show();
	m_clock.Reset();
}

void DE::Game::RunGameLoop()
{
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

			Update(deltaTime);
			Draw(deltaTime);
		}
	}
}

void DE::Game::Update(LONGLONG deltaTime)
{
}

void DE::Game::Draw(LONGLONG deltaTime)
{
	m_d3dContext.Clear();



	m_d3dContext.Present();
}
