#pragma once

namespace DE
{
	class UpdateInfo;
}

namespace DE
{
	class IGameObjectComponent
	{
	public:
		virtual			~IGameObjectComponent() = 0;

		virtual bool	IsEnabled() = 0;
		virtual void	Enable() = 0;
		virtual void	Disable() = 0;
		virtual void	Update(const UpdateInfo& info) = 0;
	};

	inline IGameObjectComponent::~IGameObjectComponent()
	{
	}
}

