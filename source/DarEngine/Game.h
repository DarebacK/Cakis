#pragma once

#include "Time.h"
#include "Clock.h"

//TODO: decouple events
namespace DarEngine
{

	class Game
	{
	public:
							Game(HINSTANCE instanceHandle, const std::wstring& windowClassName, const std::wstring& windowTitle, int showCommand);
							Game(const Game& other) = delete;
							Game& operator=(const Game& rhs) = delete;
		HINSTANCE			GetInstanceHandle() const noexcept;
		HWND				GetWindowHandle() const noexcept;
		const std::wstring&	GetWindowClassName() const noexcept;
		const std::wstring&	GetWindowTitle() const noexcept;
		int					GetScreenWidth() const noexcept;
		int					GetScreenHeight() const noexcept;
		void				SubscribeOnUpdate(const std::function<void(const Time&)>& callback); 
		void				UnsubscribeOnUpdate(const std::function<void(const Time&)>& callback);
		void				SubscribeOnDraw(const std::function<void(const Time&)>& callback);
		void				UnsubscribeOnDraw(const std::function<void(const Time&)>& callback);
		void				SubscribeOnInitialization(const std::function<void()>& callback);
		void				UnsubscribeOnInitialization(const std::function<void()>& callback);
		void				SubscribeOnExit(const std::function<void()>& callback);
		void				UnsubscribeOnExit(const std::function<void()>& callback);
		void				Run();
		void				Exit();

	private:
		HINSTANCE							m_instanceHandle{};
		std::wstring						m_windowClassName{};
		std::wstring						m_windowTitle{};
		int									m_showCommand{};
		HWND								m_windowHandle{};
		int									m_screenWidth{};
		int									m_screenHeight{};
		bool								m_isRunning{ false };
		Clock								m_clock{};
		Time								m_gameTime{};
		std::function<void(const Time&)>	m_onUpdate{};
		std::function<void(const Time&)>	m_onDraw{};
		std::function<void()>				m_onInitialization{};
		std::function<void()>				m_onExit{};

		void								Initialize();
		void								Shutdown();
		void								InvokeOnUpdate() const;
		void								InvokeOnDraw() const;
		void								InvokeOnInitialization() const;
		void								InvokeOnExit() const;
	};
}
