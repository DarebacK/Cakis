#pragma once
#include "Exception.hpp"
#include <stdio.h>

#ifndef DAR_MODULE_NAME
  #define DAR_MODULE_NAME ""
#endif

#define darAssert(condition) \
  if(!(condition)) throw De::Exception("Assertion failed: " #condition)

#define logError(message, ...) fprintf_s(stderr, "[ERROR][" DAR_MODULE_NAME "] " message "\n", __VA_ARGS__)
#define logWarning(message, ...) fprintf_s(stderr, "[WARNING][" DAR_MODULE_NAME "] " message "\n", __VA_ARGS__)
#define logInfo(messsage, ...) fprintf_s(stderr, "[INFO][" DAR_MODULE_NAME "] " message "\n", __VA_ARGS__)

#define arrayLength(arr) (sizeof(arr) / sizeof(arr[0]))