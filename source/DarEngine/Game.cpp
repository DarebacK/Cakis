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

HINSTANCE DarEngine::Game::GetInstanceHandle() const noexcept
{
	return m_instanceHandle;
}

HWND DarEngine::Game::GetWindowHandle() const noexcept
{
	return m_windowHandle;
}

const std::wstring& DarEngine::Game::GetWindowClassName() const noexcept
{
	return m_windowClassName;
}

const std::wstring& DarEngine::Game::GetWindowTitle() const noexcept
{
	return m_windowTitle;
}

int DarEngine::Game::GetScreenWidth() const noexcept
{
	return m_screenWidth;
}

int DarEngine::Game::GetScreenHeight() const noexcept
{
	return m_screenHeight;
}

void DarEngine::Game::SubscribeOnUpdate(const std::function<void(const Time&)>& callback)
{
	m_onUpdate = callback;
}

void DarEngine::Game::UnsubscribeOnUpdate(const std::function<void(const Time&)>& callback)
{
	m_onUpdate = nullptr;
}

void DarEngine::Game::SubscribeOnDraw(const std::function<void(const Time&)>& callback)
{
	m_onDraw = callback;
}

void DarEngine::Game::UnsubscribeOnDraw(const std::function<void(const Time&)>& callback)
{
	m_onDraw = nullptr;
}

void DarEngine::Game::SubscribeOnInitialization(const std::function<void()>& callback)
{
	m_onInitialization = callback;
}

void DarEngine::Game::UnsubscribeOnInitialization(const std::function<void()>& callback)
{
	m_onInitialization = nullptr;
}

void DarEngine::Game::SubscribeOnExit(const std::function<void()>& callback)
{
	m_onExit = callback;
}

void DarEngine::Game::UnsubscribeOnExit(const std::function<void()>& callback)
{
	m_onExit = nullptr;
}

void DarEngine::Game::Initialize()
{
	m_isRunning = true;
}

void DarEngine::Game::InvokeOnInitialization() const
{
	if (m_onInitialization)
	{
		m_onInitialization();
	}
}

void DarEngine::Game::InvokeOnExit() const
{
	if(m_onExit)
	{
		m_onExit();
	}
}
