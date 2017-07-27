#include "stdafx.h"
#include "Event.h"

template<typename callable_type, template<typename, typename...> typename container_type>
void DarEngine::Event<callable_type, container_type>::Subscribe(CallbackType callback)
{
	container.insert(std::move(callback.target));
}

template<typename callable_type, template<typename, typename...> typename container_type>
void DarEngine::Event<callable_type, container_type>::Unsubscribe(CallbackType callback)
{
	container.erase(std::move(callback.target));
}

template<typename callable_type, template<typename, typename...> typename container_type>
template <typename ... Args>
void DarEngine::Event<callable_type, container_type>::operator()(Args&&... arguments)
{
	for (auto i : container)
	{
		i(std::forward<Args>(arguments));
	}
}
