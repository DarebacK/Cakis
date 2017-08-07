#pragma once

#include "IGameObjectComponent.h"

namespace DE
{
	class DrawInfo;
}

namespace DE
{
	class IDrawableGameObjectComponent : public IGameObjectComponent
	{
	public:
		virtual			~IDrawableGameObjectComponent() = 0;

		virtual void	Draw(const DrawInfo& info) = 0;
	};

	inline IDrawableGameObjectComponent::~IDrawableGameObjectComponent()
	{
	}
}
