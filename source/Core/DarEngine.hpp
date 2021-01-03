#pragma once
#include <stdio.h>

using byte = unsigned char;

#ifndef DAR_MODULE_NAME
  #define DAR_MODULE_NAME ""
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

  #define logError(message, ...) \
  { \
    char stringBuffer[1024]; \
    _snprintf_s(stringBuffer, sizeof(stringBuffer), "[ERROR][" DAR_MODULE_NAME "] " message "\n", __VA_ARGS__); \
    OutputDebugStringA(stringBuffer); \
  }
  #define logWarning(message, ...) \
  { \
    char stringBuffer[1024]; \
    _snprintf_s(stringBuffer, sizeof(stringBuffer), "[WARN][" DAR_MODULE_NAME "] " message "\n", __VA_ARGS__); \
    OutputDebugStringA(stringBuffer); \
  }
  #define logInfo(message, ...) \
  { \
    char stringBuffer[1024]; \
    _snprintf_s(stringBuffer, sizeof(stringBuffer), "[INFO][" DAR_MODULE_NAME "] " message "\n", __VA_ARGS__); \
    OutputDebugStringA(stringBuffer); \
  }
#endif
#define logVariable(variable, format) logInfo(#variable " = " format, variable)

#define arrayCount(arr) (sizeof(arr) / sizeof(arr[0]))

#ifdef DAR_DEBUG
  #undef assert
  #define assert(condition) \
    if(!(condition)) { \
      logError("Assertion failed: %s", #condition); \
      *(int*)0 = 0; \
    }

  extern wchar_t _debugText[4096];
  extern int _debugTextLength;

  #define debugResetText() _debugTextLength = 0;

  void _debugStringImpl(const wchar_t* newStr, int newStrLength);
  #define debugText(...) \
  { \
    wchar_t newStr[256]; \
    int newStrLength = _snwprintf_s(newStr, _TRUNCATE, __VA_ARGS__); \
    if(newStrLength > 0) _debugStringImpl(newStr, newStrLength); \
  }
#else 
  #undef assert
  #define assert(condition) ((void)0)
  #define debugText(...)
  #define debugResetText()
#endif