#pragma once

#include "Clock.h"
#include "WinUtilities/Window.h"

namespace Dar
{

	class Game
	{
	public:
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
		WinUtilities::Window				m_window;
		Clock								m_clock{};
		bool								m_isRunning{ false };

		void								Initialize();
		void								Shutdown();
		void								InvokeOnUpdate() const;
		void								InvokeOnDraw() const;
	};


}
