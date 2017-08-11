// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma region Windows
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <wrl.h>
#include <d3d11_4.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#pragma endregion

#pragma region DirectXTK
#include "SpriteBatch.h"
#include "SpriteFont.h"
#pragma endregion

#pragma region STL
#include <string>
#include <exception>
#include <functional>
#include <memory>
#include <vector>
#include <array>
#include <set>
#include <unordered_set>
#include <map>
#include <type_traits>
#include <chrono>
#include <algorithm>
#pragma endregion

