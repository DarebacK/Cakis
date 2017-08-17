#include "stdafx.h"
#include "Game.h"
#include "Diagnostics/Exception.h"
#include "Utilities/Win32/Paths.h"
#include "Keyboard.h"

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
	}
	catch (std::exception ex)
	{
		MessageBoxA(m_window.GetHandle(), ex.what(), "Fatal error", MB_OK);
	}

	Exit();
}

void DE::Game::Exit()
{
	PostQuitMessage(0);
}

DE::Game::Game(HINSTANCE instanceHandle, const std::wstring& windowTitle, int showCommand)
	:m_instanceHandle{ instanceHandle }, m_window{ instanceHandle, windowTitle, showCommand },
	m_d3dContext{ m_window.GetHandle(), m_window.GetClientAreaWidth(), m_window.GetClientAreaHeight() },
	m_spriteDrawer{ m_d3dContext.GetDeviceContext() }, m_spriteFontDrawer{ m_d3dContext.GetDevice(), m_d3dContext.GetDeviceContext()},
	m_updateInfo{ m_highResolutionClock, m_systemClock, m_mouseState, m_mouseButtonStateTracker,
		m_keyboardState, m_keyboardStateTracker}, m_drawInfo{ m_spriteDrawer, m_spriteFontDrawer}
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	SetCurrentDirectory(Utilities::Win32::ExecutableDirectory().c_str());
	m_mouse.SetWindow(m_window.GetHandle());
}

void DE::Game::Initialize()
{
	m_window.Show();
	m_highResolutionClock.Reset();
	m_systemClock.Reset();
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
			Update();
			Draw();
		}
	}
}

void DE::Game::Update()
{
	m_highResolutionClock.Update();
	m_systemClock.Update();

	m_keyboardState = m_keyboard.GetState();
	m_keyboardStateTracker.Update(m_keyboardState);

	m_mouseState = m_mouse.GetState();
	m_mouseButtonStateTracker.Update(m_mouseState);

	for(auto& gameObjectPtr : m_gameObjects)
	{
		gameObjectPtr->OnUpdate(m_updateInfo);
	}
}

void DE::Game::PreDraw()
{
	m_spriteDrawer.OnPreDraw();
	m_spriteFontDrawer.OnPreDraw();
}

void DE::Game::Draw()
{
	m_d3dContext.Clear();
	PreDraw();

	for(auto& gameObjectPtr : m_gameObjects)
	{
		gameObjectPtr->OnDraw(m_drawInfo);
	}

	PostDraw();
	m_d3dContext.Present();
}

void DE::Game::PostDraw()
{
	m_spriteDrawer.OnPostDraw();
	m_spriteFontDrawer.OnPostDraw();
}
