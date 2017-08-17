#include "stdafx.h"
#include "Window.h"
#include "Diagnostics/Exception.h"
#include "Keyboard.h"

DE::Utilities::Win32::Window::Window(HINSTANCE instanceHandle, const std::wstring& title, int showCommand)
	:m_className{title + L"_class"}, m_title{ title }, m_showCommand{ showCommand }
{
	InitializeClass(instanceHandle);
	InitializeWindow(instanceHandle);
}

DE::Utilities::Win32::Window::~Window()
{
	DestroyWindow(m_handle);
	UnregisterClass(m_className.c_str(), m_class.hInstance);
}

void DE::Utilities::Win32::Window::Show() const
{
	ShowWindow(m_handle, m_showCommand);
	UpdateWindow(m_handle);
}

LRESULT DE::Utilities::Win32::Window::WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_ACTIVATEAPP:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	// Mouse messages
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;

	// Keyboard messages
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	default:
		break;
	}

	return DefWindowProc(windowHandle, message, wParam, lParam);
}

void DE::Utilities::Win32::Window::InitializeClass(HINSTANCE instanceHandle)
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

	if(RegisterClassEx(&m_class) == 0)
	{
		throw Diagnostics::Exception{ L"RegisterClassEx() failed for " + m_className };
	}
}

void DE::Utilities::Win32::Window::InitializeWindow(HINSTANCE instanceHandle)
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	POINT upperLeftCorner;
	upperLeftCorner.x = (screenWidth - m_clientAreaWidth) / 2;
	upperLeftCorner.y = (screenHeight - m_clientAreaHeight) / 2;

	RECT windowRectangle;
	windowRectangle = { 0, 0, m_clientAreaWidth, m_clientAreaHeight };
	AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);
	m_handle = CreateWindow(m_className.c_str(), m_title.c_str(),
		WS_OVERLAPPEDWINDOW, upperLeftCorner.x, upperLeftCorner.y, windowRectangle.right - windowRectangle.left,
		windowRectangle.bottom - windowRectangle.top,
		nullptr, nullptr, instanceHandle, nullptr);

	if(m_handle == NULL)
	{
		throw Diagnostics::Exception{ L"CreateWindow() failed for " + m_className };
	}
}