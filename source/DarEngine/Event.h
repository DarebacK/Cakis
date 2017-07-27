#pragma once

//TODO: description, template arguments requirements
//TODO: probably need void pointer or something to compare and make map instead of set
namespace DarEngine {
	template<typename callable_type, template<typename, typename...> typename container_type = std::unordered_set>
	class Event
	{
	public:
		using CallbackType = std::function<callable_type>;
		using ContainerType = container_type<callable_type>;

		void	Subscribe(CallbackType callback);
		void	Unsubscribe(CallbackType callback);
		template<typename... Args>
		void	operator()(Args&&... arguments);

	private:
		ContainerType container{};
	};

	template<typename callable_type>
	Event<callable_type>& operator+=(Event<callable_type>& event , typename Event<callable_type>::CallbackType callback)
	{
		event.Subscribe(std::move(callback));
		return event;
	}

	template<typename callable_type>
	Event<callable_type>& operator-=(Event<callable_type>& event, typename Event<callable_type>::CallbackType callback)
	{
		event.Unsubscribe(std::move(callback));
		return event;
	}

	template<typename callable_type, typename... Args>
	void Invoke(Event<callable_type>& event, Args&&... arguments)
	{
		event(std::forward<Args>(arguments));
	}
	
	void testing2();
}