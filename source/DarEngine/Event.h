#pragma once

template<typename callable_type, template<typename F> typename container_type = std::queue>
class Event
{
public:
	using CallbackType = std::function<callable_type>;
	using ContainerType = container_type<CallbackType>;
	
	void	Subscribe(CallbackType callback);
	void	Unsubscribe(CallbackType callback);
	Event&	operator+=(Event& other);
	Event&	operator-=(Event& other);
	template<typename... Args>
	void	Invoke(Args... arguments);
	template<typename... Args>
	void	operator()(Args... arguments);

private:
	ContainerType container{};
};
