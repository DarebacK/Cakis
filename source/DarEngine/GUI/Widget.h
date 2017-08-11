#pragma once


namespace DE
{
	class Game;
	class UpdateInfo;
	class DrawInfo;
}

namespace DE
{
	namespace GUI
	{
		class Widget
		{
			friend Game;

		public:
								Widget() {};
								Widget(const Widget& other) = delete;
			Widget&				operator=(const Widget& rhs) = delete;
								Widget(Widget&& other) = delete;
			Widget&				operator=(Widget&& rhs) = delete;
			virtual				~Widget() = 0;
			DirectX::XMFLOAT2	GetPosition() const noexcept { return m_position; };
			bool				IsEnabled() const noexcept { return m_isEnabled; }
			void				Enable() noexcept { m_isEnabled = true; }
			void				Disable() noexcept { m_isEnabled = false; }
			bool				IsOnUpdateEnabled() const noexcept { return m_isUpdateEnabled; }
			void				EnableOnUpdate() noexcept { m_isUpdateEnabled = true; }
			void				DisableOnUpdate() noexcept { m_isUpdateEnabled = false; }
			bool				IsOnDrawEnabled() const noexcept { return m_isOnDrawEnabled; }
			void				EnableOnDraw() noexcept { m_isOnDrawEnabled = true; }
			void				DisableOnDraw() noexcept { m_isOnDrawEnabled = false; }

		private:
			bool					m_isEnabled{ true };
			bool					m_isUpdateEnabled{ true };
			bool					m_isOnDrawEnabled{ true };
			DirectX::XMFLOAT2		m_position{};
			DirectX::SpriteBatch*	m_spriteBatch{ nullptr };

			virtual void			OnBeginPlay() {};
			virtual void			OnUpdate(const UpdateInfo& info) {};
			virtual void			OnDraw(const DrawInfo& info) {};
		};

		inline Widget::~Widget() {}
	}
}
