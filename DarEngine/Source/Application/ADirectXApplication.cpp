#include "stdafx.h"

#include "ADirectXApplication.h"

namespace
{
	// used to forward messages to user defined ProcessWindowMessage method
	ADirectXApplication* g_application = nullptr;
}

LRESULT CALLBACK MainProcessWindowMessage(_In_ HWND windowHandle, _In_ UINT uMessage, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	if(g_application)
	{
		return g_application->ProcessWindowMessage(windowHandle, uMessage, wParam, lParam);
	}

	return DefWindowProc(windowHandle, uMessage, wParam, lParam);
}

ADirectXApplication::ADirectXApplication(HINSTANCE instanceHandle, UINT clientAreaWidth, UINT clientAreaHeight, std::wstring applicationWindowTitle) :
applicationInstanceHandle(instanceHandle), 
clientAreaWidth(clientAreaWidth),
clientAreaHeight(clientAreaHeight),
applicationWindowTitle(applicationWindowTitle)
{
	g_application = this;
}

ADirectXApplication::~ADirectXApplication()
{
}

int ADirectXApplication::Run()
{
	if(IsApplicationInitialized())
	{
		OutputDebugString(L"error: trying to run the application multiple times");
		return 1;
	}

	if(!InitializeWindow())
	{
		QuitApplication(1);
	}
	OnApplicationInitialization();

	MSG lastMessage{ nullptr };
	while(lastMessage.message != WM_QUIT)
	{
		ProcessApplicationMessages(lastMessage);

		//TODO: implement delta time
		OnUpdate(0.01f);
		OnRender(0.01f);
	}

	return lastMessage.lParam;
}

LRESULT ADirectXApplication::ProcessWindowMessage(HWND windowHandle, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uMessage)
	{
	case WM_DESTROY:
		QuitApplication(0);
		return 0;

	default:
		return DefWindowProc(windowHandle, uMessage, wParam, lParam);
	}
}


void ADirectXApplication::QuitApplication(int exitCode)
{
	OnApplicationQuit();
	PostQuitMessage(exitCode);
}

bool ADirectXApplication::InitializeWindow()
{
	WNDCLASSEX windowClass;
	
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

bool ADirectXApplication::InitializeWindowClass(WNDCLASSEX& windowClass) const
{
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

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
	windowClass.lpszClassName =		L"ADirectXApplication";

	return RegisterClassEx(&windowClass);
}

HWND ADirectXApplication::CreateApplicationWindow(WNDCLASSEX windowClass) const
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

	UINT windowXCoord = GetSystemMetrics(SM_CXSCREEN) / 2 - windowWidth / 2;
	UINT windowYCoord = GetSystemMetrics(SM_CYSCREEN) / 2 - windowHeight / 2;

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

void ADirectXApplication::ProcessApplicationMessages(MSG& message)
{
	while (PeekMessage(&message, nullptr, NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

bool ADirectXApplication::IsApplicationInitialized() const
{
	return applicationWindowHandle;
}
