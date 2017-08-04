#include "stdafx.h"
#include "Game.h"
#include "Window.h"

Dar::WinUtilities::Window::Window(HINSTANCE instanceHandle, const std::wstring& title, int showCommand)
	:m_className{title + L"_class"}, m_title{ title }, m_showCommand{ showCommand }
{
	InitializeClass(instanceHandle);
	m_handle = InitializeWindow(instanceHandle);
}

void Dar::WinUtilities::Window::Show() const
{
	ShowWindow(m_handle, m_showCommand);
	UpdateWindow(m_handle);
}

LRESULT Dar::WinUtilities::Window::WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(windowHandle, message, wParam, lParam);
	}

}

void Dar::WinUtilities::Window::InitializeClass(HINSTANCE instanceHandle)
{
	ZeroMemory(&m_class, sizeof(m_class));
	m_class.cbSize = sizeof(WNDCLASSEX);
	m_class.style = CS_CLASSDC;
	m_class.lpfnWndProc = WndProc;
	m_class.hInstance = instanceHandle;
	m_class.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	m_class.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	m_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
	m_class.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
	m_class.lpszClassName = m_className.c_str();
	RegisterClassEx(&m_class);
}

HWND Dar::WinUtilities::Window::InitializeWindow(HINSTANCE instanceHandle)
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	POINT upperLeftCorner;
	upperLeftCorner.x = (screenWidth - m_clientAreaWidth) / 2;
	upperLeftCorner.y = (screenHeight - m_clientAreaHeight) / 2;

	RECT windowRectangle;
	windowRectangle = { 0, 0, m_clientAreaWidth, m_clientAreaHeight };
	AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);
	return CreateWindow(m_className.c_str(), m_title.c_str(),
		WS_OVERLAPPEDWINDOW, upperLeftCorner.x, upperLeftCorner.y, windowRectangle.right - windowRectangle.left,
		windowRectangle.bottom - windowRectangle.top,
		nullptr, nullptr, instanceHandle, nullptr);
}