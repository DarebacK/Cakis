#pragma once

#include "Component.h"

namespace DE
{
	class DrawInfo;
}

namespace DE
{
	class DrawableComponent : public Component
	{
	public:
		virtual			~DrawableComponent() = 0;

		bool			IsOnDrawEnabled() const noexcept { return m_isDrawEnabled; }
		void			EnableOnDraw() noexcept { m_isDrawEnabled = true; }
		void			DisableOnDraw() noexcept { m_isDrawEnabled = false; }

	private:
		bool			m_isDrawEnabled{ true };

		virtual void	OnDraw(const DrawInfo& info) {};
	};

	inline DrawableComponent::~DrawableComponent() {}
}
