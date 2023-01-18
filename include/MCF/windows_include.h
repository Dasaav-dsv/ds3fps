
/* Mod Compatibility Framework
 * Copyright (c) 2022 William Tremblay.
 
 * This program is free software; licensed under the MIT license. 
 * You should have received a copy of the license along with this program. 
 * If not, see <https://opensource.org/licenses/MIT>.
 */

#pragma once

// Prevent the Windows macros from colliding with std::max/std::min
#define NOMINMAX
// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>