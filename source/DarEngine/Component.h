#pragma once

namespace DE
{
	class UpdateInfo;
	class DrawInfo;
	class GameObject;
}

namespace DE
{
	class Component
	{
		friend	GameObject;	// to set m_parentGameObject

	public:
		virtual			~Component() = 0;

		bool			IsEnabled() const noexcept { return m_isEnabled; }
		void			Enable() noexcept { m_isEnabled = true; }
		void			Disable() noexcept { m_isEnabled = false; }
		bool			IsOnUpdateEnabled() const noexcept { return m_isOnUpdateEnabled; }
		void			EnableOnUpdate() noexcept { m_isOnUpdateEnabled = true; }
		void			DisableOnUpdate() noexcept { m_isOnUpdateEnabled = false; }
		bool			IsOnDrawEnabled() const noexcept { return m_isOnDrawEnabled; }
		void			EnableOnDraw() noexcept { m_isOnDrawEnabled = true; }
		void			DisableOnDraw() noexcept { m_isOnDrawEnabled = false; }
		GameObject&		GetParentGameObject() const noexcept { return *m_parentGameObject; }

	private:
		GameObject*		m_parentGameObject{ nullptr };
		bool			m_isEnabled{ true };
		bool			m_isOnUpdateEnabled{ true };
		bool			m_isOnDrawEnabled{ true };

		virtual void	OnBeginPlay() {};
		virtual void	OnUpdate(const UpdateInfo& info) {};
		virtual void	OnDraw(const DrawInfo& info) {};

	};

	inline Component::~Component()
	{
	}
}

