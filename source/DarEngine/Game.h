#pragma once

#include "Clock.h"
#include "Event.h"

//TODO: decouple events
//TODO: make getters implicitly inline
namespace DarEngine
{

	class Game
	{
	public:
		Event<void(LONGLONG)>		m_onUpdate{};
		Event<void(LONGLONG)>		m_onDraw{};
		Event<void()>				m_onInitialization{};
		Event<void()>				m_onExit{};

									Game(HINSTANCE instanceHandle, const std::wstring& windowClassName, const std::wstring& windowTitle, int showCommand);
									Game(const Game& other) = delete;
									Game(Game&& other) = default;
									Game& operator=(const Game& rhs) = delete;
									Game& operator=(Game&& rhs) = default;
									~Game() = default;

		int							GetWindowWidth() const noexcept { return m_windowWidth; }
		int							GetWindowHeight() const noexcept { return m_windowHeight; }
		void						Run();
		void						Exit();

	private:
		static const int			windowDefaultWidth{ 800 };
		static const int			windowDefaultHeight{ 600 };

		HINSTANCE					m_instanceHandle{};
		std::wstring				m_windowClassName{};
		std::wstring				m_windowTitle{};
		int							m_showCommand{};
		HWND						m_windowHandle{};
		WNDCLASSEX					m_window{};
		int							m_windowWidth{ windowDefaultWidth };
		int							m_windowHeight{ windowDefaultHeight };
		bool						m_isRunning{ false };
		Clock						m_clock{};

		void						Initialize();
		void						InitializeWindow();
		void						Shutdown();
		void						InvokeOnUpdate() const;
		void						InvokeOnDraw() const;
		void						InvokeOnInitialization() const;
		void						InvokeOnExit() const;
		static	LRESULT WINAPI		WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	};
}
