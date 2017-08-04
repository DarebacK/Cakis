#include "stdafx.h"
#include "Exception.h"


DE::Exception::Exception()
	:message{}
{
}

DE::Exception::Exception(const std::wstring& message)
	:message{message}
{
}

DE::Exception::Exception(const wchar_t* message)
	:message{message}
{
}