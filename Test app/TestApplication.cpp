#include "TestApplication.h"

TestApplication::TestApplication(HINSTANCE instanceHandle) :
	ADirectXApplication(instanceHandle, 800, 600, L"TestApplication")
{
}

bool TestApplication::Update(float deltaTime)
{
	return true;
}

bool TestApplication::Render(float deltaTIme)
{
	return true;
}

void TestApplication::OnApplicationInitialization()
{
	OutputDebugString(L"derived initialize");
}

void TestApplication::OnApplicationQuit()
{
}
