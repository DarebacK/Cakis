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

	InitializeWindow();
	InvokeOnInitialization();
}


void DarEngine::Game::InitializeWindow()
{
	ZeroMemory(&m_window, sizeof(m_window));
	m_window.cbSize = sizeof(WNDCLASSEX);
	m_window.style = CS_CLASSDC;
	m_window.lpfnWndProc = WndProc;
	m_window.hInstance = m_instanceHandle;
	m_window.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	m_window.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	m_window.hCursor = LoadCursor(nullptr, IDC_ARROW);
	m_window.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
	m_window.lpszClassName = m_windowClassName.c_str();
	RECT windowRectangle = { 0, 0, m_windowWidth, m_windowHeight };
	AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);
	RegisterClassEx(&m_window);

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	POINT center;
	center.x = (screenWidth - m_windowWidth) / 2;
	center.y = (screenHeight - m_windowHeight) / 2;

	m_windowHandle = CreateWindow(m_windowClassName.c_str(), m_windowTitle.c_str(),
		WS_OVERLAPPEDWINDOW, center.x, center.y, windowRectangle.right - windowRectangle.left,
		windowRectangle.bottom - windowRectangle.top,
		nullptr, nullptr, m_instanceHandle, nullptr);

	ShowWindow(m_windowHandle, m_showCommand);
	UpdateWindow(m_windowHandle);
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

LRESULT DarEngine::Game::WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(windowHandle, message, wParam, lParam);
	}
	
}
