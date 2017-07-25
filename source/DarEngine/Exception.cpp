#include "stdafx.h"
#include "Exception.h"


DarEngine::Exception::Exception()
	:message{}
{
}

DarEngine::Exception::Exception(const std::wstring& message)
	:message{message}
{
}

DarEngine::Exception::Exception(const wchar_t* message)
	:message{message}
{
}