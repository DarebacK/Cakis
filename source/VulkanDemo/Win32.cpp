#include <windows.h>
#include <exception>
#include "VulkanRenderer.h"

namespace 
{
int clientAreaWidth = 1280;
int clientAreaHeight = 720;
const DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
HWND windowHandle = nullptr;
}

static LRESULT CALLBACK WindowProc(HWND   windowHandle,
                                   UINT   message,
                                   WPARAM wParam,
                                   LPARAM lParam)
{
  LRESULT result = 0;
  switch(message)
  {
    case WM_SIZE:
      clientAreaWidth = lParam & 0xFFFF;
      clientAreaHeight = lParam >> 16;
      break;
    default:
      result = DefWindowProcA(windowHandle, message, wParam, lParam);
      break;
  }
  return result;
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
	AdjustWindowRect(&windowRectangle, windowStyle, FALSE);
  windowHandle = CreateWindowA(windowClass.lpszClassName, 
                                    "VulkanDemo", 
                                    windowStyle,
                                    CW_USEDEFAULT, 
                                    CW_USEDEFAULT,
                                    windowRectangle.right - windowRectangle.left, 
                                    windowRectangle.bottom - windowRectangle.top,
                                    nullptr, 
                                    nullptr,
                                    windowClass.hInstance,
                                    nullptr);

  if(!initVulkanRenderer(instanceHandle, windowHandle))
  {
    MessageBoxA(windowHandle, "Failed to initialize Vulkan renderer.", "Fatal error", MB_OK | MB_ICONERROR);
    return -1;
  }

  MSG message;
  BOOL result;
  while((result = GetMessageA(&message, windowHandle, 0, 0)) != 0)
  {
    if(result == -1)
    {
      return result;
    }
    else 
    {
      TranslateMessage(&message);
      DispatchMessageA(&message);
    }
  }
}
catch(const std::exception e)
{
  MessageBoxA(windowHandle, e.what(), "Fatal error", MB_OK | MB_ICONERROR);
}
catch(...)
{
  MessageBoxA(windowHandle, "Unknown error", "Fatal error", MB_OK | MB_ICONERROR);
}