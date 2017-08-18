#pragma once

#include "Timing/Clock.h"
#include "Utilities/Win32/Window.h"
#include "Utilities/DirectX11/D3DContext.h"
#include "GameObject.h"
#include "Sprite.h"
#include "SpriteText.h"
#include "UpdateInfo.h"
#include "DrawInfo.h"
#include "Camera.h"

//TODO: extract keyboard and mouse stuff, add option to change mouse mode
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
		int				GetClientAreaWidth() const { return m_window.GetClientAreaWidth(); }
		int				GetClientAreaHeight() const { return m_window.GetClientAreaWidth(); }

	private:
		DirectX::Mouse						m_mouse{};
		DirectX::Mouse::State				m_mouseState{};
		DirectX::Mouse::ButtonStateTracker	m_mouseButtonStateTracker{};
		DirectX::Keyboard					m_keyboard{};
		DirectX::Keyboard::State			m_keyboardState{};
		DirectX::Keyboard::KeyboardStateTracker m_keyboardStateTracker{};
		Timing::Clock<std::chrono::high_resolution_clock>	m_highResolutionClock{};
		Timing::Clock<std::chrono::system_clock>			m_systemClock{};
		std::vector<std::unique_ptr<GameObject>>			m_gameObjects{};
		unsigned long						m_gameObjectIdCounter{ 0 }; //TODO: refactor this outside GameClass, something like GameObjectManager
		HINSTANCE							m_instanceHandle{ nullptr };
		Utilities::Win32::Window			m_window;
		Utilities::DirectX11::D3DContext	m_d3dContext;
		SpriteDrawer						m_spriteDrawer;
		SpriteTextDrawer					m_spriteFontDrawer;
		UpdateInfo							m_updateInfo;
		DrawInfo							m_drawInfo;
		
		void Initialize();
		void RunGameLoop();
		void Update();
		void PreDraw();
		void Draw();
		void PostDraw();
	};

	template <typename GameObjectType, typename ... Args>
	GameObjectType* Game::AddGameObjectByType(Args... arguments)
	{
		m_gameObjects.push_back(std::make_unique<GameObjectType>(std::forward<Args>(arguments)...));
		GameObjectType* newGameObjectPtr = static_cast<GameObjectType*>(m_gameObjects.back().get());
		newGameObjectPtr->m_id = m_gameObjectIdCounter++;
		newGameObjectPtr->m_parentGame = this;

		// ignore syntax error highlight, it's c++17 feature
		if constexpr(std::is_base_of<Camera, GameObjectType>::value)
		{
			newGameObjectPtr->SetAspectRatio(GetAspectRatio(m_window));
		}

		return newGameObjectPtr;
	}

	inline float GetAspectRatio(const DE::Game& game)
	{
		return float(game.GetClientAreaWidth()) / game.GetClientAreaHeight();
	}
}
