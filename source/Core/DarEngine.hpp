#pragma once
#include <stdio.h>

#include "Exception.hpp"
#include "DarMath.hpp"

using byte = unsigned char;

#ifndef DAR_MODULE_NAME
  #define DAR_MODULE_NAME ""
#endif

#define logError(message, ...) \
{ \
  char stringBuffer[256]; \
  _snprintf_s(stringBuffer, sizeof(stringBuffer), "[ERROR][" DAR_MODULE_NAME "] " message "\n", __VA_ARGS__); \
  OutputDebugStringA(stringBuffer); \
}
#define logWarning(message, ...) \
{ \
  char stringBuffer[256]; \
  _snprintf_s(stringBuffer, sizeof(stringBuffer), "[WARN][" DAR_MODULE_NAME "] " message "\n", __VA_ARGS__); \
  OutputDebugStringA(stringBuffer); \
}
#define logInfo(message, ...) \
{ \
  char stringBuffer[256]; \
  _snprintf_s(stringBuffer, sizeof(stringBuffer), "[INFO][" DAR_MODULE_NAME "] " message "\n", __VA_ARGS__); \
  OutputDebugStringA(stringBuffer); \
}

#define arrayCount(arr) (sizeof(arr) / sizeof(arr[0]))

#ifdef DAR_DEBUG
  #define darAssert(condition) \
    if(!(condition)) \
    { \
      logError("Assertion failed: %s", #condition); \
      DebugBreak(); \
    }

  extern wchar_t _debugTextString[4096];
  extern int _debugTextStringLength;

  void _debugStringImpl(const wchar_t* newStr, int newStrLength);
  #define debugString(...) \
  { \
    wchar_t newStr[256]; \
    int newStrLength = _snwprintf_s(newStr, _TRUNCATE, __VA_ARGS__); \
    if(newStrLength > 0) _debugStringImpl(newStr, newStrLength); \
  }
#else 
  #define darAssert(condition) condition
  #define debugString(...)
#endif