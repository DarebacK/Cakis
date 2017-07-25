#pragma once

namespace DarEngine
{
	class Exception
	{
	public:
		std::wstring message;

		Exception();
		explicit Exception(const std::wstring& message);
		explicit Exception(const wchar_t* message);
		virtual ~Exception() = default;
	};
}