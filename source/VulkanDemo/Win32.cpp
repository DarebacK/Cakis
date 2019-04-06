#include <windows.h>
#include "VulkanRenderer.h"

namespace 
{
int clientAreaWidth = 1280;
int clientAreaHeight = 720;
const DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
}

static LRESULT CALLBACK WindowProc(_In_ HWND   windowHandle,
                                   _In_ UINT   message,
                                   _In_ WPARAM wParam,
                                   _In_ LPARAM lParam)
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
{
  WNDCLASS windowClass{};
  windowClass.lpfnWndProc = &WindowProc;
  windowClass.hInstance = instanceHandle;
  windowClass.lpszClassName = "What's the point of this struct";
  if(!RegisterClassA(&windowClass)) return -1;
  RECT windowRectangle;
	windowRectangle = { 0, 0, clientAreaWidth, clientAreaHeight };
	AdjustWindowRect(&windowRectangle, windowStyle, FALSE);
  HWND windowHandle = CreateWindowA(windowClass.lpszClassName, 
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

  initVulkanRenderer(windowHandle);

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