#include "stdafx.h"
#include "Game.h"
#include "Exception.h"

#if defined(DEBUG) || defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

using namespace DarEngine;

int WINAPI wWinMain(HINSTANCE instanceHandle, HINSTANCE previousInstanceHandle, PWSTR commandLine, int showCommand)
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Game game{ instanceHandle, L"DemoGameClass", L"DarEngine Demo", showCommand };
	
	try
	{
		game.Run();
	}
	catch(Exception ex)
	{
		MessageBox(game.GetWindowHandle(), ex.message.c_str(), game.GetWindowTitle().c_str(), MB_ABORTRETRYIGNORE);
	}

	return 0;
}