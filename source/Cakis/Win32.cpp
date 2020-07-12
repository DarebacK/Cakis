#define DAR_MODULE_NAME "Win32"

#include <exception>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <Psapi.h>

#include "D3D11Renderer.hpp"
#include "DarEngine.hpp"
#include "GameState.hpp"
#include "Audio.hpp"

namespace 
{
int clientAreaWidth = GetSystemMetrics(SM_CXSCREEN);
int clientAreaHeight = GetSystemMetrics(SM_CYSCREEN);
HWND window = nullptr;
HANDLE process = nullptr;
WINDOWPLACEMENT windowPosition = {sizeof(windowPosition)};
const char* gameName = "Demo";
Input input = {};
int processorCount = 0;

LRESULT CALLBACK WindowProc(
  HWND   windowHandle,
  UINT   message,
  WPARAM wParam,
  LPARAM lParam
)
{
  LRESULT result = 0;
  switch(message) {
    case WM_SIZE: {
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
      switch(wParam) {
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

static void debugShowResourcesUsage()
{
#ifdef DAR_DEBUG
  // from https://stackoverflow.com/a/64166/9178254

  // CPU
  static ULARGE_INTEGER lastCPU = {}, lastSysCPU = {}, lastUserCPU = {};
  static double percent = 0.;
  static DWORD lastCheckTimeMs = 0;
  DWORD currentTimeMs = GetTickCount();
  if((currentTimeMs - lastCheckTimeMs) > 1000) {
    ULARGE_INTEGER now, sys, user;
    GetSystemTimeAsFileTime((LPFILETIME)&now);
    FILETIME fileTime;
    GetProcessTimes(process, &fileTime, &fileTime, (LPFILETIME)&sys, (LPFILETIME)&user);
    percent = (sys.QuadPart - lastSysCPU.QuadPart) +
        (user.QuadPart - lastUserCPU.QuadPart);
    percent /= (now.QuadPart - lastCPU.QuadPart);
    percent /= processorCount;
    percent *= 100;
    lastCPU = now;
    lastUserCPU = user;
    lastSysCPU = sys;
    lastCheckTimeMs = currentTimeMs;
  }
  debugText(L"CPU %.f%%", percent);

  // Memory
  MEMORYSTATUSEX memoryStatus;
  memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
  GlobalMemoryStatusEx(&memoryStatus);
  DWORDLONG totalVirtualMemoryMB = memoryStatus.ullTotalPageFile / (1ull << 20ull);
  DWORDLONG totalPhysicalMemoryMB = memoryStatus.ullTotalPhys / (1ull << 20ull);
  PROCESS_MEMORY_COUNTERS_EX processMemoryCounters;
  GetProcessMemoryInfo(process, (PROCESS_MEMORY_COUNTERS*)&processMemoryCounters, sizeof(processMemoryCounters));
  SIZE_T virtualMemoryUsedByGameMB = processMemoryCounters.PrivateUsage / (1ull << 20ull);
  SIZE_T physicalMemoryUsedByGameMB = processMemoryCounters.WorkingSetSize / (1ull << 20ull);
  debugText(L"Virtual memory %llu MB / %llu MB", virtualMemoryUsedByGameMB, totalVirtualMemoryMB);
  debugText(L"Physical memory %llu MB / %llu MB", physicalMemoryUsedByGameMB, totalPhysicalMemoryMB);

#endif
}

static void showErrorMessageBox(const char* text, const char* caption) 
{
  MessageBoxA(window, text, caption, MB_OK | MB_ICONERROR);
}

int WINAPI WinMain(
  HINSTANCE instanceHandle,
  HINSTANCE hPrevInstance, // always zero
  LPSTR commandLine,
  int showCode
)
try
{
  process = GetCurrentProcess();

  SYSTEM_INFO sysInfo;
  GetSystemInfo(&sysInfo);
  processorCount = sysInfo.dwNumberOfProcessors;

  WNDCLASS windowClass{};
  windowClass.lpfnWndProc = &WindowProc;
  windowClass.hInstance = instanceHandle;
  windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  windowClass.lpszClassName = "Game window class";
  if(!RegisterClassA(&windowClass)) {
    showErrorMessageBox("Failed to register window class.", "Fatal error");
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
    showErrorMessageBox("Failed to create game window", "Fatal error");
    return -1;
  }

  if(!Renderer::init(window)) {
    showErrorMessageBox("Failed to initialize D3D11 renderer.", "Fatal error");
    return -1;
  }

  bool isAudioInitialized = Audio::initialize();

  ShowWindow(window, SW_SHOWNORMAL);

  LARGE_INTEGER counterFrequency;
  QueryPerformanceFrequency(&counterFrequency);
  LARGE_INTEGER lastCounterValue;
  QueryPerformanceCounter(&lastCounterValue);
  MSG message{};
  while (message.message != WM_QUIT) {
    if(PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&message);
      DispatchMessageA(&message);
    } else {
      // process frame

      GameState gameState{};
      gameState.input = input;
      gameState.clientAreaWidth = clientAreaWidth;
      gameState.clientAreaHeight = clientAreaHeight;

      LARGE_INTEGER currentCounterValue;
      QueryPerformanceCounter(&currentCounterValue);
      gameState.dTime = (float)(currentCounterValue.QuadPart - lastCounterValue.QuadPart) / counterFrequency.QuadPart;
      lastCounterValue = currentCounterValue;

      debugResetText();
      debugText(L"%.3f ms / %d fps", gameState.dTime, (int)(1/gameState.dTime));
      debugShowResourcesUsage();

      Renderer::render(gameState);

      if(isAudioInitialized) {
        Audio::update(gameState);
      }

      input = {};
    }
  }
  return 0;
} catch(const std::exception& e) {
  showErrorMessageBox(e.what(), "Fatal error");
  return -2;
} catch(...) {
  showErrorMessageBox("Unknown error", "Fatal error");
  return -2;
}
