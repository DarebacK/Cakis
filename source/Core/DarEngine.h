#pragma once
#include "Exception.hpp"
#include <stdio.h>

using byte = unsigned char;

#ifndef DAR_MODULE_NAME
  #define DAR_MODULE_NAME ""
#endif

#ifdef DAR_DEBUG
  #define darAssert(condition) \
    if(!(condition)) DebugBreak();
#else 
  #define darAssert(condition) condition
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