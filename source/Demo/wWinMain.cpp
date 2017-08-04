#include "stdafx.h"
#include "Game.h"

using namespace DE;

int WINAPI wWinMain(HINSTANCE instanceHandle, HINSTANCE previousInstanceHandle, PWSTR commandLine, int showCommand)
{
	Game game{ instanceHandle, L"DE Demo", showCommand };	
	game.Run();

	return 0;
}