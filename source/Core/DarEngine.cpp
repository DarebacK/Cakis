#include "DarEngine.hpp"

wchar_t _debugTextString[4096];
int _debugTextStringLength = 0;

#ifdef DAR_DEBUG
void _debugStringImpl(const wchar_t* newStr, int newStrLength)
{
  int newDebugTextStringLength = _debugTextStringLength + newStrLength;
  newDebugTextStringLength = clamp(newDebugTextStringLength, 0, (int)arrayCount(_debugTextString));
  wchar_t* debugTextStringOffset = _debugTextString + _debugTextStringLength;
  int remainingDebugTextStringSpace =  arrayCount(_debugTextString) - _debugTextStringLength;
  if(remainingDebugTextStringSpace > 0)
  {
    _snwprintf_s(debugTextStringOffset, arrayCount(_debugTextString) - _debugTextStringLength, _TRUNCATE, L"%s\n", newStr);
    _debugTextStringLength = newDebugTextStringLength + 1;
  }
}
#endif