#pragma once
#include <memory>

namespace De
{
	class Library
	{
	public:
		explicit	Library(const char* fileName);
					Library(const Library& other) = delete;
					Library(Library&& other) = default;
					~Library();
		Library&	operator=(const Library& rhs) = delete;
		Library&	operator=(Library&& rhs) = default;
		void*	loadFunction(const char* name);
	private:
		class Impl;
		std::unique_ptr<Impl> pImpl;
	};
}
