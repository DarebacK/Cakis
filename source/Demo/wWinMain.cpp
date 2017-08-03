#include "stdafx.h"
#include "Game.h"

using namespace Dar;

int WINAPI wWinMain(HINSTANCE instanceHandle, HINSTANCE previousInstanceHandle, PWSTR commandLine, int showCommand)
{
	Game game{ instanceHandle, L"Dar Demo", showCommand };	
	game.Run();

	return 0;
}