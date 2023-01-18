// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#include "../dinput8/targetver.h"

#define NOMINMAX						// Prevent the Windows macros from colliding with std::max/std::min
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <winternl.h>
#include <imagehlp.h>

#define DINPUT8_API __declspec(dllexport)

// reference additional headers your program requires here

#include "psapi.h"
#include "memoryapi.h"
#include "processthreadsapi.h"

#include <stdexcept>
#include <iostream>
#include <map>

#include "Defs.h"