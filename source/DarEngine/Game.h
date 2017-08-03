#pragma once

#include "Clock.h"
#include "Event.h"
#include "Win32/Window.h"

namespace Dar
{

	class Game
	{
	public:
		Event<void(LONGLONG)>	OnUpdate;
		Event<void(LONGLONG)>	OnDraw;
		Event<void()>			OnInitialization;
		Event<void()>			OnExit;

								Game(HINSTANCE instanceHandle, const std::wstring& windowTitle, int showCommand);
								Game(const Game& other) = delete;
								Game(Game&& other) = default;
								Game& operator=(const Game& rhs) = delete;
								Game& operator=(Game&& rhs) = default;
								~Game() = default;
		void					Run();
		void					Exit();

	private:
		HINSTANCE							m_instanceHandle{};
		Win32::Window						m_window;
		Clock								m_clock{};
		Event<void(LONGLONG)>::InvokerType	m_onUpdateInvoker{};
		Event<void(LONGLONG)>::InvokerType	m_onDrawInvoker{};
		Event<void()>::InvokerType			m_onInitializationInvoker{};
		Event<void()>::InvokerType			m_onExitInvoker{};
		bool								m_isRunning{ false };

		void								Initialize();
		void								Shutdown();
		void								InvokeOnUpdate() const;
		void								InvokeOnDraw() const;
	};


}
