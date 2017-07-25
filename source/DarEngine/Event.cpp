#include "stdafx.h"
#include "Event.h"

template <typename callable_type, template <typename F> class container_type>
void Event<callable_type, container_type>::Subscribe(CallbackType callback)
{
}

template <typename callable_type, template <typename F> class container_type>
void Event<callable_type, container_type>::Unsubscribe(CallbackType callback)
{
}

template <typename callable_type, template <typename F> class container_type>
Event<callable_type, container_type>& Event<callable_type, container_type>::operator+=(Event& other)
{
}

template <typename callable_type, template <typename F> class container_type>
Event<callable_type, container_type>& Event<callable_type, container_type>::operator-=(Event& other)
{
}

template <typename callable_type, template <typename F> class container_type>
template <typename ... Args>
void Event<callable_type, container_type>::Invoke(Args... arguments)
{
}

template <typename callable_type, template <typename F> class container_type>
template <typename ... Args>
void Event<callable_type, container_type>::operator()(Args... arguments)
{
}
