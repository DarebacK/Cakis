#include "stdafx.h"

#include "TestApplication.h"

int CALLBACK wWinMain(
	_In_		HINSTANCE		instanceHandle,
	_In_opt_	HINSTANCE		previousInstanceHandle,
	_In_		PWSTR			lpCommandLine,
	_In_		int				nCommandShow
)
{
	TestApplication testApplication{instanceHandle};

	return testApplication.Run();
}
