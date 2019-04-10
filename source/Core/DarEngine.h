#pragma once
#include "Exception.hpp"

#define darAssert(condition) \
  if(!(condition)) throw De::Exception("Assertion failed: " #condition)