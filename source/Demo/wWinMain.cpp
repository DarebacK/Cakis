#include "stdafx.h"
#include "Game.h"

using namespace DarEngine;

int WINAPI wWinMain(HINSTANCE instanceHandle, HINSTANCE previousInstanceHandle, PWSTR commandLine, int showCommand)
{
	Game game{ instanceHandle, L"DemoGameClass", L"DarEngine Demo", showCommand };	
	game.Run();

	return 0;
}