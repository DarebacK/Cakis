#include "stdafx.h"
#include "Exception.h"


Dar::Exception::Exception()
	:message{}
{
}

Dar::Exception::Exception(const std::wstring& message)
	:message{message}
{
}

Dar::Exception::Exception(const wchar_t* message)
	:message{message}
{
}