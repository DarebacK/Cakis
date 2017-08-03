#pragma once

//TODO: description, template arguments requirements
//TODO: specialize std::hash and std::equal_to
namespace DarEngine {
	template<typename callable_type>
	class Event;

	template<typename return_type, typename... args>
	class Event<return_type(args...)>
	{
	public:
		using CallbackType	= std::function<return_type(args...)>;
		using InvokerType	= std::function<void(args...)>;

		explicit Event(InvokerType& out_invoker)
		{
			out_invoker = [this](args&&... arguments)
			{
				for (auto i : callbacks)
				{
					i(std::forward<args>(arguments)...);
				}
			};
		}
		void	Subscribe(CallbackType callback);
		void	Unsubscribe(CallbackType callback);
		

	private:
		std::set<CallbackType>	callbacks{};
	};


	template <typename return_type, typename ... args>
	void Event<return_type(args...)>::Subscribe(CallbackType callback)
	{
		callbacks.insert(std::move(callback));
	}

	template <typename return_type, typename ... args>
	void Event<return_type(args...)>::Unsubscribe(CallbackType callback)
	{
		callbacks.erase(std::move(callback));
	}
}
