#include "stdafx.h"
#include "Game.h"
#include "TimeInfoDisplayer.h"

using namespace DE;

int WINAPI wWinMain(HINSTANCE instanceHandle, HINSTANCE previousInstanceHandle, PWSTR commandLine, int showCommand)
{
	Game game{ instanceHandle, L"DE Demo", showCommand };
	game.AddGameObjectByType<GameObject>()->AddComponentByType<Demo::TimeInfoDisplayer>();
	game.Run();

	return 0;
}