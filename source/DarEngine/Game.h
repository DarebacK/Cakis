#pragma once

#include "Time.h"
#include "Clock.h"
#include "Event.h"

//TODO: decouple events
//TODO: make getters implicitly inline
namespace DarEngine
{

	class Game
	{
	public:
		Event<void(const Time&)>	m_onUpdate{};
		Event<void(const Time&)>	m_onDraw{};
		Event<void()>				m_onInitialization{};
		Event<void()>				m_onExit{};

									Game(HINSTANCE instanceHandle, const std::wstring& windowClassName, const std::wstring& windowTitle, int showCommand);
									Game(const Game& other) = delete;
									Game(Game&& other) = default;
									Game& operator=(const Game& rhs) = delete;
									Game& operator=(Game&& rhs) = default;
									~Game() = default;
		//HINSTANCE			GetInstanceHandle() const noexcept { return m_instanceHandle; }
		//HWND				GetWindowHandle() const noexcept { return m_windowHandle; }
		//const std::wstring&	GetWindowClassName() const noexcept { return m_windowClassName; }
		//const std::wstring&	GetWindowTitle() const noexcept { return m_windowTitle; }
		int							GetScreenWidth() const noexcept { return m_screenWidth; }
		int							GetScreenHeight() const noexcept { return m_screenHeight; }
		void						Run();
		void						Exit();

	private:
		HINSTANCE					m_instanceHandle{};
		std::wstring				m_windowClassName{};
		std::wstring				m_windowTitle{};
		int							m_showCommand{};
		HWND						m_windowHandle{};
		int							m_screenWidth{};
		int							m_screenHeight{};
		bool						m_isRunning{ false };
		Clock						m_clock{};

		void						Initialize();
		void						Shutdown();
		void						InvokeOnUpdate() const;
		void						InvokeOnDraw() const;
		void						InvokeOnInitialization() const;
		void						InvokeOnExit() const;
	};
}
