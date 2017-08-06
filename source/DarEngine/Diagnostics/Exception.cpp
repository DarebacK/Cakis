#include "stdafx.h"
#include "Exception.h"

DE::Diagnostics::Exception::Exception()
	:message{}
{
}

DE::Diagnostics::Exception::Exception(const std::wstring& message)
	: message{ message }
{
}

DE::Diagnostics::Exception::Exception(const wchar_t* message)
	: message{ message }
{
}