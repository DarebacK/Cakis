#pragma once

#include "Component.h"

namespace DE
{
	class Game;
	class UpdateInfo;
	class DrawInfo;
}

namespace DE
{
	//Class which exists in the world (scene), holds it's components.
	//Has unique Id which is used to compare it to other GameObjects.
	class GameObject
	{
		friend Game;

	public:
						GameObject() = default;
						GameObject(const GameObject& other) = delete;
		GameObject&		operator=(const GameObject& rhs) = delete;
						GameObject(GameObject&& other) = default;
		GameObject&		operator=(GameObject&& other) = default;
		virtual			~GameObject() = default;
		auto			GetId() const noexcept { return m_id; };
		void			OnUpdate(const UpdateInfo& info);
		void			OnDraw(const DrawInfo& info);
		//Creates and attaches a component to this GameObject
		//The component type must derive from Component
		template<typename ComponentType, typename... Args>
		ComponentType*	AddComponentByType(Args... arguments);
		// Gets contained component by type.
		// Returns pointer to the component if found, nullptr otherwise.
		// Note: Is performance demanding, does dynamic_cast
		// for every contained component until found.
		template<typename ComponentType>
		ComponentType*	GetComponentByType();
	
	private:
		unsigned long							m_id;
		std::vector<std::unique_ptr<Component>>	m_components;
	};


	template <typename ComponentType, typename ... Args>
	ComponentType* GameObject::AddComponentByType(Args... arguments)
	{
		m_components.push_back(std::make_unique<ComponentType>(std::forward<Args>(arguments)...));
		return static_cast<ComponentType*>(m_components.back().get());
	}

	template <typename ComponentType>
	ComponentType* GameObject::GetComponentByType()
	{
		ComponentType* returnValue{ nullptr };

		for (auto& i : m_components)
		{
			if (returnValue = dynamic_cast<ComponentType*>(i.get()))
			{
				break;
			}
		}

		return returnValue;
	}


	inline bool	operator==(const GameObject& lhs, const GameObject& rhs)
	{
		return lhs.GetId() == rhs.GetId();
	}
	inline bool	operator<(const GameObject& lhs, const GameObject& rhs)
	{
		return lhs.GetId() < rhs.GetId();
	}
	inline bool	operator>(const GameObject& lhs, const GameObject& rhs)
	{
		return lhs.GetId() > rhs.GetId();
	}

}

namespace std
{
	template<>
	struct hash<DE::GameObject>
	{
		size_t operator()(const DE::GameObject& x) const { return hash<decltype(x.GetId())>()(x.GetId()); }
	};
	
}