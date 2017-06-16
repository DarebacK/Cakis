#include "stdafx.h"
#include "DirectX11Application.h"

DarEngine::DirectX11Application::DirectX11Application(HINSTANCE instanceHandle, UINT clientAreaWidth, UINT clientAreaHeight, const std::wstring& applicationWindowTitle)
	:Win32Application(instanceHandle, clientAreaWidth, clientAreaHeight, applicationWindowTitle)
{
}

DarEngine::DirectX11Application::~DirectX11Application()
{
}

void DarEngine::DirectX11Application::OnMessageLoopTick()
{
}
