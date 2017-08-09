#pragma once

#include "IGameObjectComponent.h"
#include "IDrawableGameObjectComponent.h"

namespace DE
{
	//Class which exists in the world (scene), holds it's components.
	//Has unique Id which is used to compare it to other GameObjects
	class GameObject
	{
	public:
						GameObject() = default;
						GameObject(const GameObject& other) = delete;
		GameObject&		operator=(const GameObject& rhs) = delete;
						GameObject(GameObject&& other) = delete;
		GameObject&		operator=(GameObject&& other) = delete;
		virtual			~GameObject() = default;
		unsigned long	GetId() const { return m_id; };
		//Creates and attaches a component to this GameObject.
		//The component type must derive from one of the
		//supported component types : IGameObjectComponent, IDrawableGameObjectComponent.
		template<typename ComponentType, typename... Args>
		typename std::enable_if<std::is_base_of<IDrawableGameObjectComponent, ComponentType>::value, ComponentType>::type*	AddComponent(Args... arguments);
		//Creates and attaches a component to this GameObject.
		//The component type must derive from one of the
		//supported component types : IGameObjectComponent, IDrawableGameObjectComponent.
		template<typename ComponentType, typename... Args>
		typename std::enable_if<std::is_base_of<IGameObjectComponent, ComponentType>::value && !std::is_base_of<IDrawableGameObjectComponent, ComponentType>::value, ComponentType>::type*	AddComponent(Args... arguments);
	
	private:
		unsigned long							m_id;
		std::set<IGameObjectComponent>			m_components;
		std::set<IDrawableGameObjectComponent>	m_drawableComponents;
	};


	template <typename ComponentType, typename... Args>
	typename std::enable_if<std::is_base_of<IDrawableGameObjectComponent, ComponentType>::value, ComponentType>::type* GameObject::AddComponent(Args... arguments)
	{
		return m_drawableComponents.emplace(std::forward<Args>(arguments)...);
	}

	template <typename ComponentType, typename... Args>
	typename std::enable_if<std::is_base_of<IGameObjectComponent, ComponentType>::value && !std::is_base_of<IDrawableGameObjectComponent, ComponentType>::value, ComponentType>::type* GameObject::AddComponent(Args... arguments)
	{
		return m_components.emplace(std::forward<Args>(arguments)...);
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