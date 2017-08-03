#pragma once

//TODO: description, template arguments requirements
//TODO: specialize std::hash and std::equal_to
//TODO: move invocation out so users don't have to encapsulate this
namespace DarEngine {
	template<typename callable_type, template<typename, typename...> typename container_type = std::unordered_set>
	class Event
	{
	public:
		using CallbackType = std::function<callable_type>;
		using ContainerType = container_type<CallbackType>;

		void	Subscribe(CallbackType callback);
		void	Unsubscribe(CallbackType callback);
		template<typename... Args>
		void	Invoke(Args&&... arguments);

	private:
		ContainerType container{};
	};


	template <typename callable_type, template <typename, typename ...> class container_type>
	void Event<callable_type, container_type>::Subscribe(CallbackType callback)
	{
		container.insert(std::move(callback)); 
	}

	template <typename callable_type, template <typename, typename ...> class container_type>
	void Event<callable_type, container_type>::Unsubscribe(CallbackType callback)
	{
		container.erase(std::move(callback));
	}

	template <typename callable_type, template <typename, typename ...> class container_type>
	template <typename ... Args>
	void Event<callable_type, container_type>::Invoke(Args&&... arguments)
	{
		for (auto i : container)
		{
			i(std::forward<Args>(arguments)...);
		}
	}
}
