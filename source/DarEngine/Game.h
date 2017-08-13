#pragma once

#include "Timing/Clock.h"
#include "Utilities/Win32/Window.h"
#include "Utilities/DirectX11/D3DContext.h"
#include "GameObject.h"
#include "Sprite.h"
#include "SpriteText.h"
#include "UpdateInfo.h"
#include "DrawInfo.h"

namespace DE
{
	class Game
	{
	public:
						Game(HINSTANCE instanceHandle, const std::wstring& windowTitle, int showCommand);
						Game(const Game& other) = delete;
		Game&			operator=(const Game& rhs) = delete;
						Game(Game&& other) = default;
		Game&			operator=(Game&& rhs) = default;
						~Game() = default;
		void			Run();
		void			Exit();
		template<typename GameObjectType, typename... Args>
		GameObjectType*	AddGameObjectByType(Args... arguments);

	private:
		HINSTANCE							m_instanceHandle{ nullptr };
		Utilities::Win32::Window			m_window;
		Utilities::DirectX11::D3DContext	m_d3dContext;
		Timing::Clock						m_clock{};
		std::vector<GameObject>				m_gameObjects{};
		SpriteDrawer						m_spriteDrawer;
		SpriteTextDrawer					m_spriteFontDrawer;
		UpdateInfo							m_updateInfo;
		DrawInfo							m_drawInfo;
		unsigned long						m_gameObjectIdCounter{ 0 }; //TODO: refactor this outside GameClass, something like GameObjectManager
		
		void								Initialize();
		void								RunGameLoop();
		void								Update();
		void								PreDraw();
		void								Draw();
		void								PostDraw();
	};


	template <typename GameObjectType, typename ... Args>
	GameObjectType* Game::AddGameObjectByType(Args... arguments)
	{
		auto newGameObject = m_gameObjects.emplace_back(std::forward<Args>(arguments)...);
		newGameObject.m_id = m_gameObjectIdCounter++;
	}
}
