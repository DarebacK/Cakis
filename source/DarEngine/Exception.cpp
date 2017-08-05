#include "stdafx.h"
#include "Exception.h"

namespace DE
{
Exception::Exception()
	:message{}
{
}

Exception::Exception(const std::wstring& message)
	: message{ message }
{
}

Exception::Exception(const wchar_t* message)
	: message{ message }
{
}
}