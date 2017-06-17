#include "stdafx.h"

#include "Win32Application.h"
#include "Platform/Metrics.h"

namespace
{
	// used to forward messages to user defined ProcessWindowMessage method
	DarEngine::Win32Application* g_application = nullptr;
}

LRESULT CALLBACK MainProcessWindowMessage(_In_ HWND windowHandle, _In_ UINT uMessage, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	if(g_application)
	{
		return g_application->ProcessWindowMessage(windowHandle, uMessage, wParam, lParam);
	}

	return DefWindowProc(windowHandle, uMessage, wParam, lParam);
}

DarEngine::Win32Application::Win32Application(HINSTANCE instanceHandle, UINT clientAreaWidth, UINT clientAreaHeight, std::wstring applicationWindowTitle) :
applicationInstanceHandle(instanceHandle), 
clientAreaWidth(clientAreaWidth),
clientAreaHeight(clientAreaHeight),
applicationWindowTitle(applicationWindowTitle)
{
	g_application = this;
}

int DarEngine::Win32Application::Run()
{
	if (IsApplicationInitialized())
	{
		OutputDebugString(L"error: trying to run the application multiple times");
		return 1;
	}
	
	if (!InitializeWindow())
	{
		QuitApplication(1);
		return 1;
	}
	OnApplicationInitialization();

	MSG lastMessage{ nullptr };
	while (lastMessage.message != WM_QUIT)
	{
		ProcessApplicationMessages(lastMessage);
		OnMessageLoopTick();
	}

	return lastMessage.lParam;
}

LRESULT DarEngine::Win32Application::ProcessWindowMessage(HWND windowHandle, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	return DoProcessWindowMessage(windowHandle, uMessage, wParam, lParam);
}

UINT DarEngine::Win32Application::GetClientAreaWidth() const
{
	return clientAreaWidth;
}

UINT DarEngine::Win32Application::GetClientAreaHeight() const
{
	return clientAreaHeight;
}

const std::wstring& DarEngine::Win32Application::GetAppplicationWindowTitle() const
{
	return applicationWindowTitle;
}

const HWND DarEngine::Win32Application::GetApplicationWindowHandle() const
{
	return applicationWindowHandle;
}

LRESULT DarEngine::Win32Application::DoProcessWindowMessage(HWND windowHandle, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_DESTROY:
		QuitApplication(0);
		return 0;

	default:
		return DefWindowProc(windowHandle, uMessage, wParam, lParam);
	}
}

void DarEngine::Win32Application::QuitApplication(int exitCode)
{
	OnApplicationQuit();
	PostQuitMessage(exitCode);
}

bool DarEngine::Win32Application::InitializeWindow()
{
	WNDCLASSEX windowClass{};
	if(!InitializeWindowClass(windowClass))
	{
		OutputDebugString(L"error: window class initialization failed");
		return false;
	}

	applicationWindowHandle = CreateApplicationWindow(windowClass);
	if(!applicationWindowHandle)
	{
		OutputDebugString(L"error: window creation failed");
		return false;
	}

	ShowWindow(applicationWindowHandle, SW_SHOW);

	return true;
}

bool DarEngine::Win32Application::InitializeWindowClass(WNDCLASSEX& windowClass) const
{
	windowClass.cbSize =			sizeof(WNDCLASSEX);
	windowClass.cbClsExtra =		0;
	windowClass.cbWndExtra =		0;
	windowClass.style =				CS_HREDRAW | CS_VREDRAW;
	windowClass.hInstance =			applicationInstanceHandle;
	windowClass.lpfnWndProc =		MainProcessWindowMessage;
	windowClass.hIcon =				LoadIcon(nullptr, IDI_APPLICATION);	//TODO: custom icon
	windowClass.hIconSm =			LoadIcon(nullptr, IDI_APPLICATION);
	windowClass.hCursor =			LoadCursor(nullptr, IDC_ARROW);
	windowClass.hbrBackground =		static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
	windowClass.lpszMenuName =		nullptr;
	windowClass.lpszClassName =		L"Win32Application";

	return RegisterClassEx(&windowClass);
}

HWND DarEngine::Win32Application::CreateApplicationWindow(WNDCLASSEX windowClass) const
{
	RECT windowRectangle{
		0,
		0,
		static_cast<LONG>(clientAreaWidth),
		static_cast<LONG>(clientAreaHeight)
	};
	AdjustWindowRect(&windowRectangle, applicationWindowStyle, NULL);

	UINT windowWidth = windowRectangle.right - windowRectangle.left;
	UINT windowHeight = windowRectangle.bottom - windowRectangle.top;

	UINT windowXCoord = DarEngine::GetScreenWidth() / 2 - windowWidth / 2;
	UINT windowYCoord = DarEngine::GetScreenHeight() / 2 - windowHeight / 2;

	return CreateWindow(
		windowClass.lpszClassName,
		applicationWindowTitle.c_str(),
		applicationWindowStyle,
		windowXCoord,
		windowYCoord,
		windowWidth,
		windowHeight,
		nullptr,
		nullptr,
		applicationInstanceHandle,
		NULL
	);
}

void DarEngine::Win32Application::ProcessApplicationMessages(MSG& message)
{
	while (PeekMessage(&message, nullptr, NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

bool DarEngine::Win32Application::IsApplicationInitialized() const
{
	return applicationWindowHandle;
}
