#define DAR_MODULE_NAME "Win32"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <exception>
#include <stdio.h>

#include "D3D11Renderer.hpp"
#include "DarEngine.hpp"
#include "GameState.hpp"

namespace 
{
int clientAreaWidth = GetSystemMetrics(SM_CXSCREEN);
int clientAreaHeight = GetSystemMetrics(SM_CYSCREEN);
HWND window = nullptr;
WINDOWPLACEMENT windowPosition = {sizeof(windowPosition)};
const char* gameName = "Demo";
Input input = {};

LRESULT CALLBACK WindowProc(
  HWND   windowHandle,
  UINT   message,
  WPARAM wParam,
  LPARAM lParam
)
{
  LRESULT result = 0;
  switch(message)
  {
    case WM_SIZE:
    {
      int newClientAreaWidth = LOWORD(lParam);
      int newClientAreaHeight = HIWORD(lParam);
      if(newClientAreaWidth != clientAreaWidth || newClientAreaHeight != clientAreaHeight) {
        clientAreaWidth = newClientAreaWidth;
        clientAreaHeight = newClientAreaHeight;
        Renderer::onWindowResize(clientAreaWidth, clientAreaHeight);
      }
    }
    break;
    case WM_DESTROY:
      PostQuitMessage(0);
    break;
    case WM_LBUTTONDOWN:
      input.mouse.left.pressedDown = true;
    break;
    case WM_LBUTTONUP:
      input.mouse.left.pressedUp = true;
    break;
    case WM_MBUTTONDOWN:
      input.mouse.middle.pressedDown = true;
    break;
    case WM_MBUTTONUP:
      input.mouse.middle.pressedUp = true;
    break;
    case WM_RBUTTONDOWN:
      input.mouse.right.pressedDown = true;
    break;
    case WM_RBUTTONUP:
      input.mouse.right.pressedUp = true;
    break;
    case WM_MOUSEMOVE:
      input.mouse.x = GET_X_LPARAM(lParam);
      input.mouse.y = GET_Y_LPARAM(lParam);
    break;
    case WM_KEYDOWN:
      switch(wParam)
      {
        case VK_ESCAPE :
          PostQuitMessage(0);
        break;
        case VK_F1:
          input.F1.pressedDown = true;
        break;
        case VK_MENU:
          input.rightAlt.pressedDown = true;
        break;
        case VK_RETURN:
          input.enter.pressedDown = true;
        break;
      }
    break;
    default:
      result = DefWindowProc(windowHandle, message, wParam, lParam);
    break;
  }
  return result;
}
}

int WINAPI WinMain(
  HINSTANCE instanceHandle,
  HINSTANCE hPrevInstance, // always zero
  LPSTR commandLine,
  int showCode
)
try
{
  WNDCLASS windowClass{};
  windowClass.lpfnWndProc = &WindowProc;
  windowClass.hInstance = instanceHandle;
  windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  windowClass.lpszClassName = "Game window class";
  if(!RegisterClassA(&windowClass)) {
    MessageBoxA(nullptr, "Failed to register window class.", "Fatal error", MB_OK | MB_ICONERROR);
    return -1;
  }

  RECT windowRectangle = { 0, 0, clientAreaWidth, clientAreaHeight };
  constexpr DWORD windowStyle = WS_POPUP;
  constexpr DWORD windowStyleEx = 0;
  AdjustWindowRectEx(&windowRectangle, windowStyle, false, windowStyleEx);
  const int windowWidth = windowRectangle.right - windowRectangle.left;
  const int windowHeight = windowRectangle.bottom - windowRectangle.top;
  window = CreateWindowExA(
    windowStyleEx, 
    windowClass.lpszClassName, 
    gameName, 
    windowStyle,
    CW_USEDEFAULT, 
    CW_USEDEFAULT,
    windowWidth, 
    windowHeight,
    nullptr, 
    nullptr,
    windowClass.hInstance,
    nullptr
  );
  if(!window) {
    MessageBoxA(nullptr, "Failed to create game window", "Fatal error", MB_OK | MB_ICONERROR);
    return -1;
  }

  if(!Renderer::init(window))
  {
    MessageBoxA(window, "Failed to initialize D3D11 renderer.", "Fatal error", MB_OK | MB_ICONERROR);
    return -1;
  }

  ShowWindow(window, SW_SHOWNORMAL);

  LARGE_INTEGER counterFrequency;
  QueryPerformanceFrequency(&counterFrequency);
  LARGE_INTEGER lastCounterValue;
  QueryPerformanceCounter(&lastCounterValue);
  MSG message{};
  while (message.message != WM_QUIT)
  {
    if(PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&message);
      DispatchMessageA(&message);
    }
    else
    {
      // process frame
      GameState gameState{};
      gameState.input = input;
      gameState.clientAreaWidth = clientAreaWidth;
      gameState.clientAreaHeight = clientAreaHeight;

      LARGE_INTEGER currentCounterValue;
      QueryPerformanceCounter(&currentCounterValue);
      gameState.dTime = (float)(currentCounterValue.QuadPart - lastCounterValue.QuadPart) / counterFrequency.QuadPart;
      lastCounterValue = currentCounterValue;

      #ifdef DAR_DEBUG
        _debugTextStringLength = 0;
      #endif 

      Renderer::render(gameState);

      input = {};
    }
  }
  return 0;
}
catch(const std::exception& e)
{
  MessageBoxA(window, e.what(), "Fatal error", MB_OK | MB_ICONERROR);
  return -2;
}
catch(...)
{
  MessageBoxA(window, "Unknown error", "Fatal error", MB_OK | MB_ICONERROR);
  return -2;
}
