#pragma once

#include "Clock.h"
#include "Event.h"

namespace DarEngine
{

	class Game
	{
	public:
									Game(HINSTANCE instanceHandle, const std::wstring& windowClassName, const std::wstring& windowTitle, int showCommand);
									Game(const Game& other) = delete;
									Game(Game&& other) = default;
									Game& operator=(const Game& rhs) = delete;
									Game& operator=(Game&& rhs) = default;
									~Game() = default;

		int							GetClientAreaWidth() const noexcept { return m_clientAreaWidth; }
		int							GetClientAreaHeight() const noexcept { return m_clientAreaHeight; }
		void						Run();
		void						Exit();

	private:
		static const int			clientAreaDefaultWidth{ 800 };
		static const int			clientAreaDefaultHeight{ 600 };

		HINSTANCE							m_instanceHandle{};
		std::wstring						m_windowClassName{};
		std::wstring						m_windowTitle{};
		int									m_showCommand{};
		HWND								m_windowHandle{};
		WNDCLASSEX							m_window{};
		int									m_clientAreaWidth{ clientAreaDefaultWidth };
		int									m_clientAreaHeight{ clientAreaDefaultHeight };
		bool								m_isRunning{ false };
		Clock								m_clock{};
		Event<void(LONGLONG)>::InvokerType	m_onUpdateInvoker;
		Event<void(LONGLONG)>::InvokerType	m_onDrawInvoker;
		Event<void()>::InvokerType			m_onInitializationInvoker;
		Event<void()>::InvokerType			m_onExitInvoker;

		void						Initialize();
		void						InitializeWindow();
		void						Shutdown();
		void						InvokeOnUpdate() const;
		void						InvokeOnDraw() const;
		static	LRESULT WINAPI		WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

	public:
		Event<void(LONGLONG)>		OnUpdate{m_onUpdateInvoker};
		Event<void(LONGLONG)>		OnDraw{m_onDrawInvoker};
		Event<void()>				OnInitialization{m_onInitializationInvoker};
		Event<void()>				OnExit{m_onExitInvoker};
	};


}
