#include <windows.h>
#include <exception>
#include <stdio.h>
#include "VulkanRenderer.h"

namespace 
{
int clientAreaWidth = 1280;
int clientAreaHeight = 720;
HWND windowHandle = nullptr;

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
    default:
      result = DefWindowProcA(windowHandle, message, wParam, lParam);
    break;
  }
  return result;
}
}

#ifdef DAR_DEBUG
int main(int argc, char** argv)
try
{
  HINSTANCE instanceHandle = GetModuleHandle(nullptr);
#else
int WINAPI WinMain(HINSTANCE instanceHandle,
                   HINSTANCE hPrevInstance, // always zero
                   LPSTR     commandLine,
                   int       showCode)
try
{
#endif
  WNDCLASS windowClass{};
  windowClass.lpfnWndProc = &WindowProc;
  windowClass.hInstance = instanceHandle;
  windowClass.lpszClassName = "What's the point of this struct";
  if(!RegisterClassA(&windowClass)) return -1;
  RECT windowRectangle;
  windowRectangle = { 0, 0, clientAreaWidth, clientAreaHeight };
  constexpr DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
  AdjustWindowRect(&windowRectangle, windowStyle, FALSE);
  int windowWidth = windowRectangle.right - windowRectangle.left;
  int windowHeight = windowRectangle.bottom - windowRectangle.top;
  int screenWidth = GetSystemMetrics(SM_CXSCREEN);
  int screenHeight = GetSystemMetrics(SM_CYSCREEN);
  windowHandle = CreateWindowA(windowClass.lpszClassName, 
                               "VulkanDemo", 
                               windowStyle,
                               (screenWidth - windowWidth) / 2, 
                               0,
                               windowWidth, 
                               windowHeight,
                               nullptr, 
                               nullptr,
                               windowClass.hInstance,
                               nullptr);
  #ifdef DAR_DEBUG
    HWND consoleWindow = GetConsoleWindow();
    MoveWindow(consoleWindow, (screenWidth - windowWidth) / 2, clientAreaHeight, windowWidth, screenHeight - clientAreaHeight + 8, false);
  #endif
  if(!initVulkanRenderer(instanceHandle, windowHandle))
  {
    MessageBoxA(windowHandle, "Failed to initialize Vulkan renderer.", "Fatal error", MB_OK | MB_ICONERROR);
    return -1;
  }

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
      float frameTime = (float)(currentCounterValue.QuadPart - lastCounterValue.QuadPart) / counterFrequency.QuadPart;
      lastCounterValue = currentCounterValue;
      //char frameTimeString[32];
      //_snprintf_s(frameTimeString, 32, "%.2fms\n", 1000 * frameTime);
      //OutputDebugStringA(frameTimeString);

      // process frame
      rendererPresent();
    }
  }
  return 0;
}
catch(const std::exception& e)
{
  MessageBoxA(windowHandle, e.what(), "Fatal error", MB_OK | MB_ICONERROR);
}
catch(...)
{
  MessageBoxA(windowHandle, "Unknown error", "Fatal error", MB_OK | MB_ICONERROR);
}
