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
	std::wstring windowTitle{ L"DarEngine Demo" };
	Game game{ instanceHandle, L"DemoGameClass", windowTitle, showCommand };
	
	try
	{
		game.Run();
	}
	catch(Exception ex)
	{
		MessageBox(game.GetWindowHandle(), ex.message.c_str(), L"Fatal error", MB_ABORTRETRYIGNORE);
	}

	return 0;
}