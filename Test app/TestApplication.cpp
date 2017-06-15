#include "stdafx.h"

#include "TestApplication.h"

TestApplication::TestApplication(HINSTANCE instanceHandle) :
	DarEngine::ADirectXApplication(instanceHandle, 800, 600, L"TestApplication")
{
}

void TestApplication::OnUpdate(float deltaTime)
{

}

void TestApplication::OnRender(float deltaTime)
{

}

void TestApplication::OnApplicationInitialization()
{
	OutputDebugString(L"derived initialize");
}

void TestApplication::OnApplicationQuit()
{

}
