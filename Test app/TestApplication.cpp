#include "TestApplication.h"

TestApplication::TestApplication(HINSTANCE instanceHandle) :
	ADirectXApplication(instanceHandle, 800, 600, L"TestApplication")
{
}

void TestApplication::OnUpdate(float deltaTime)
{
}

void TestApplication::OnRender(float deltaTIme)
{
}

void TestApplication::OnApplicationInitialization()
{
	OutputDebugString(L"derived initialize");
}

void TestApplication::OnApplicationQuit()
{
}
