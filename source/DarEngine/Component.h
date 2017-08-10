#pragma once

namespace DE
{
	class UpdateInfo;
}

namespace DE
{
	class Component
	{
	public:
		virtual			~Component() = 0;

		bool			IsEnabled() const noexcept { return m_isEnabled; }
		void			Enable() noexcept { m_isEnabled = true; }
		void			Disable() noexcept { m_isEnabled = false; }
		bool			IsOnUpdateEnabled() const noexcept { return m_isUpdateEnabled; }
		void			EnableOnUpdate() noexcept { m_isUpdateEnabled = true; }
		void			DisableOnUpdate() noexcept { m_isUpdateEnabled = false; }

	private:
		bool	m_isEnabled{ true };
		bool	m_isUpdateEnabled{ true };

		virtual void	OnBeginPlay() {};
		virtual void	OnFirstUpdate(const UpdateInfo& info) {};
		virtual void	OnUpdate(const UpdateInfo& info) {};

	};

	inline Component::~Component() {}
}

