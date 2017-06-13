#pragma once

#include <functional>

class Object
{
public:

	explicit		Object(long id);

	long			GetId()	const noexcept;
private:
	long			id;
};

// default compare
namespace std
{
	template<>
	struct less<Object>
	{
		bool		operator()(const Object& lhs, const Object& rhs);
	};
}