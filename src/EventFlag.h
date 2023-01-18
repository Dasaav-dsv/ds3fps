#pragma once

extern void GetEventFunctions(const char* pGetFlag, const char* pSetFlag);

extern __declspec(dllexport) bool __cdecl DefGetEventFlag(const void* Manager, const unsigned int FlagID);

extern __declspec(dllexport) void __cdecl DefSetEventFlag(const void* Manager, const unsigned int FlagID, const bool State, const char Param_4);

extern decltype(&::DefGetEventFlag) GetEventFlag;

extern decltype(&::DefSetEventFlag) SetEventFlag;