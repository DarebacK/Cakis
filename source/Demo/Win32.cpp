#define DAR_MODULE_NAME "Win32"
#include <windows.h>
#include <exception>
#include <stdio.h>
#include "D3D11Renderer.h"
#include "DarEngine.h"
#include "GameState.h"

namespace 
{
int clientAreaWidth = 1280;
int clientAreaHeight = 720;
HWND window = nullptr;
const char* gameName = "Demo";
Input input = {};

LRESULT CALLBACK WindowProc(HWND   windowHandle,
                            UINT   message,
                            WPARAM wParam,
                            LPARAM lParam)
{
  LRESULT result = 0;
  switch(message)
  {
    case WM_SIZE:
      clientAreaWidth = lParam & 0xFFFF;
      clientAreaHeight = (int)lParam >> 16;
    break;
    case WM_DESTROY:
      PostQuitMessage(0);
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
      }
    break;
    default:
      result = DefWindowProcA(windowHandle, message, wParam, lParam);
    break;
  }
  return result;
}
}

int WINAPI WinMain(HINSTANCE instanceHandle,
                   HINSTANCE hPrevInstance, // always zero
                   LPSTR     commandLine,
                   int       showCode)
try
{
  WNDCLASS windowClass{};
  windowClass.lpfnWndProc = &WindowProc;
  windowClass.hInstance = instanceHandle;
  windowClass.lpszClassName = "What's the point of this struct";
  if(!RegisterClassA(&windowClass)) return -1;
  RECT windowRectangle;
  windowRectangle = { 0, 0, clientAreaWidth, clientAreaHeight };
  constexpr DWORD windowStyle = WS_OVERLAPPEDWINDOW ^ WS_SIZEBOX;
  constexpr DWORD windowStyleEx = WS_EX_OVERLAPPEDWINDOW ^ WS_SIZEBOX;
  AdjustWindowRectEx(&windowRectangle, windowStyle, false, windowStyleEx);
  const int windowWidth = windowRectangle.right - windowRectangle.left;
  const int windowHeight = windowRectangle.bottom - windowRectangle.top;
  const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
  const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
  window = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, windowClass.lpszClassName, 
                         gameName, 
                         windowStyle,
                         (screenWidth - windowWidth) / 2, 
                         0,
                         windowWidth, 
                         windowHeight,
                         nullptr, 
                         nullptr,
                         windowClass.hInstance,
                         nullptr);
  if(!initD3D11Renderer(window))
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
      LARGE_INTEGER currentCounterValue;
      QueryPerformanceCounter(&currentCounterValue);
      const float frameTime = (float)(currentCounterValue.QuadPart - lastCounterValue.QuadPart) / counterFrequency.QuadPart;
      lastCounterValue = currentCounterValue;
      char windowTitle[64];
      _snprintf_s(windowTitle, 64, "%s %.2fms/%dfps", gameName, frameTime, (int)(1/frameTime));
      SetWindowTextA(window, windowTitle);

      // process frame
      GameState gameState{};
      gameState.input = input;
      render(gameState);

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
  return -3;
}