#include "File.hpp"

#include "Exception.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
namespace De
{
  void readEntireFile(const char* fileName, std::vector<uint8_t>& buffer)
  {
    HANDLE file = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if(file == INVALID_HANDLE_VALUE) {
      throw Exception(std::string("Failed to load shader file ") + fileName);
    }
    LARGE_INTEGER fileSize;
    if(!GetFileSizeEx(file, &fileSize)) {
      CloseHandle(file);
      throw Exception(std::string("Failed to get file size of shader file ") + fileName);
    }
    buffer.reserve(fileSize.QuadPart);
    DWORD bytesRead;
    if(!ReadFile(file, buffer.data(), (DWORD)fileSize.QuadPart, &bytesRead, nullptr)) {
      CloseHandle(file);
      throw Exception(std::string("Failed to read shader file ") + fileName);
    }
    buffer.resize(bytesRead);
    CloseHandle(file);
  }
}
#endif