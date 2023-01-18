
/* Mod Compatibility Framework
 * Copyright (c) 2022 William Tremblay.
 
 * This program is free software; licensed under the MIT license. 
 * You should have received a copy of the license along with this program. 
 * If not, see <https://opensource.org/licenses/MIT>.
 */

#pragma once
#include "windows_include.h"
#include "MemRegion.h"

namespace MCF
{
    /// Tries to allocate a block of memory using VirtualAlloc such that region is within INT_MAX of any address
    /// in the allocated region. To free, use the address written to alloc_base.
    inline void* VirtualAllocNear(MemRegion region, size_t size, DWORD protection_flags, void*& alloc_base, DWORD alloc_granularity)
    {
        // Search free region closest to target module to allocate our hook memory at,
        // starting at lowest possible address that admits a REL32 jmp
        MEMORY_BASIC_INFORMATION mbi;
        mbi.BaseAddress = (PVOID)(region.begin + size - INT_MAX);
        mbi.RegionSize = 0;

        while (VirtualQuery((PVOID)((uintptr_t)mbi.BaseAddress + mbi.RegionSize), &mbi, sizeof(mbi)))
        {
            if (!(mbi.State & MEM_FREE)) continue;
            if (!((uintptr_t)mbi.BaseAddress % alloc_granularity)) continue;
            if (size > mbi.RegionSize) continue;

            auto mem_begin = (uintptr_t)mbi.BaseAddress;
            auto mem_end = mem_begin + mbi.RegionSize;

            if (mem_end <= region.begin && region.end - (mem_end - size) > INT_MAX) continue;
            if (mem_begin >= region.end && mem_begin + size - region.begin > INT_MAX) break;

            // Found suitable region, allocate
            void* desired = (LPVOID)(mem_end <= region.begin ? mem_end - size : mem_begin);
            alloc_base = VirtualAlloc(desired, size, MEM_COMMIT | MEM_RESERVE, protection_flags);
            return alloc_base ? desired : nullptr;
        }
        return nullptr;
    }
}