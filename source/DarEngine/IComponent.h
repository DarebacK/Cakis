#pragma once

namespace DE
{
	class UpdateInfo;
}

namespace DE
{
	class IComponent
	{
	public:
		virtual			~IComponent() = 0;

		virtual void	Update(const UpdateInfo& info) = 0;
		virtual bool	IsEnabled() = 0;
		virtual void	Enable() = 0;
		virtual void	Disable() = 0;
	};

	inline IComponent::~IComponent()
	{
	}
}

