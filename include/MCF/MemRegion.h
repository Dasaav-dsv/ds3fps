
/* Mod Compatibility Framework
 * Copyright (c) 2022 William Tremblay.
 
 * This program is free software; licensed under the MIT license. 
 * You should have received a copy of the license along with this program. 
 * If not, see <https://opensource.org/licenses/MIT>.
 */

#pragma once
#include <cstdint>
#include "windows_include.h"
#include <psapi.h>
#include <algorithm>

namespace MCF
{
    /// IBO address (32-bit image offset). Direct pointer on x86 and RVA from module base on x64.
    using ibo32 = int32_t;

    struct MemRegion
    {
        uintptr_t begin = 0;
        uintptr_t end = 0;

        /// Base address of the DLL / EXE containing this memory region. Zero if the region is not contained by a module.
        uintptr_t module_base = 0;

        MemRegion() = default;

        MemRegion(uintptr_t begin, uintptr_t end, uintptr_t module_base) :
            begin(begin), end(end), module_base(module_base) { }

        MemRegion(uintptr_t begin, uintptr_t end) : begin(begin), end(end)
        {
            HMODULE hmodule;
            if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                    reinterpret_cast<LPCSTR>(begin), &hmodule)) {
                MODULEINFO minfo{};
                if (GetModuleInformation(GetCurrentProcess(), hmodule, &minfo, sizeof(minfo))) {
                    module_base = reinterpret_cast<uintptr_t>(minfo.lpBaseOfDll);
                    if (end - module_base > minfo.SizeOfImage) module_base = 0;
                }
            }
        }

        template<typename T>
        MemRegion(const T* begin, const T* end) :
                begin(reinterpret_cast<uintptr_t>(begin)),
                end(reinterpret_cast<uintptr_t>(end)) { }

        /// Create a MemRegion from a module given it's name. Passing a null pointer defaults to the main module.
        explicit MemRegion(const char* module_name)
        {
            HMODULE hmodule = GetModuleHandleA(module_name);
            MODULEINFO minfo{};
            if (GetModuleInformation(GetCurrentProcess(), hmodule, &minfo, sizeof(minfo))) {
                begin = module_base = reinterpret_cast<uintptr_t>(minfo.lpBaseOfDll);
                end = begin + minfo.SizeOfImage;
            }
        }

        /// Create MemRegion from a module section given the section and module names.
        MemRegion(const char* module_name, const char* pe_section)
        {
            HMODULE hmodule = GetModuleHandleA(module_name);
            MODULEINFO minfo{};
            if (!GetModuleInformation(GetCurrentProcess(), hmodule, &minfo, sizeof(minfo)))
                return;

            auto base = reinterpret_cast<uintptr_t>(minfo.lpBaseOfDll);

            auto dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
            auto nt_header = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos_header->e_lfanew);
            auto sections = IMAGE_FIRST_SECTION(nt_header);

            for (int i = 0; i < nt_header->FileHeader.NumberOfSections; i++) {
                auto section = sections + i;
                if (strcmp(reinterpret_cast<const char*>(section->Name), pe_section) == 0) {
                    //if constexpr(sizeof(void*) == 4) begin = section->VirtualAddress;
                    begin = base + section->VirtualAddress;
                    end = begin + section->Misc.VirtualSize;
                    module_base = base;
                    return;
                }
            }
        }

        /// Check if the object pointed at by ptr fits in this memory region.
        template<typename T>
        inline bool contains(T* ptr) const
        {
            auto as_int = reinterpret_cast<uintptr_t>(ptr);
            return begin <= as_int && as_int + sizeof(T) <= end;
        }

        inline bool contains(const MemRegion& region) const
        {
            return begin <= region.begin && end >= region.end;
        }

        inline MemRegion combine(const MemRegion& region) const
        {
            if (module_base != region.module_base) return {};
            else return {std::min(begin, region.begin), std::max(end, region.end), module_base};
        }

        /// Convert a relative virtual address to a pointer, with bounds checking.
        /// If the bounds checking fails, returns a null pointer.
        template<typename T>
        inline T* rva2ptr(int32_t rva) const
        {
            auto ptr = reinterpret_cast<T*>(module_base + rva);
            return contains(ptr) ? ptr : nullptr;
        }

        /// Convert an IBO32 address to a pointer. No-op on x86. Does not check if the address lies in the memory region.
        template<typename T>
        inline T* ibo2ptr_unchecked(ibo32 ibo) const
        {
            if constexpr (sizeof(void*) == 4) return reinterpret_cast<T*>(ibo);
            else return reinterpret_cast<T*>(module_base + ibo);
        }

        /// Convert an IBO32 address to a pointer, and also checks if the address fits in the region. Returns nullptr otherwise.
        template<typename T>
        inline T* ibo2ptr(ibo32 ibo) const
        {
            auto ptr = ibo2ptr_unchecked<T>(ibo);
            return contains(ptr) ? ptr : nullptr;
        }

        inline size_t size() const { return end - begin; }
    };

    /// Get a memory region that encompasses the entire process's address space.
    inline const MemRegion& EntireAddressSpace()
    {
        static MemRegion all(0, INTPTR_MAX, 0);
        return all;
    }

    /// Get the memory region corresponding to the main module of this process.
    inline const MemRegion& MainModule()
    {
        static MemRegion main(nullptr);
        return main;
    }

    /// Get the memory region corresponding to the main module's .text (code) section.
    inline const MemRegion& MainModuleText()
    {
        static MemRegion main_text(nullptr, ".text");
        return main_text;
    }

    /// Get the memory region corresponding to the main module's .rdata (read-only initialized data, e.g. RTTI) section.
    inline const MemRegion& MainModuleRdata()
    {
        static MemRegion main_rdata(nullptr, ".rdata");
        return main_rdata;
    }

    /// Get the memory region corresponding to the main module's .data (static uninitialized data) section.
    inline const MemRegion& MainModuleData()
    {
        static MemRegion main_data(nullptr, ".data");
        return main_data;
    }
}

