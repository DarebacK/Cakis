#pragma once

#include "IComponent.h"

namespace DE
{
	class DrawInfo;
}

namespace DE
{
	class IDrawableComponent : public IComponent
	{
	public:
		virtual			~IDrawableComponent() = 0;

		virtual void	Draw(const DrawInfo& info) = 0;
	};

	inline IDrawableComponent::~IDrawableComponent()
	{
	}
}
