#pragma once

#include "Timing/Clock.h"
#include "Utilities/Win32/Window.h"
#include "Utilities/DirectX11/D3DContext.h"
#include "GameObject.h"

//TODO: pImpl to hide Dx and Win32 stuff
namespace DE
{
	class Game
	{
	public:
								Game(HINSTANCE instanceHandle, const std::wstring& windowTitle, int showCommand);
								Game(const Game& other) = delete;
		Game&					operator=(const Game& rhs) = delete;
								Game(Game&& other) = default;
		Game&					operator=(Game&& rhs) = default;
								~Game() = default;
		void					Run();
		void					Exit();

	private:
		HINSTANCE							m_instanceHandle{ nullptr };
		Utilities::Win32::Window			m_window;
		Utilities::DirectX11::D3DContext	m_d3dContext;
		Timing::Clock						m_clock{};
		std::unordered_set<GameObject>		m_gameObjects{};
		
		void								Initialize();
		void								RunGameLoop();
		void								Update();
		void								Draw();
	};


}
