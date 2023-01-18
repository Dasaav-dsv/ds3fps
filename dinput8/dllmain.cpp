// dllmain.cpp : Defines the entry point for the DLL application.
#include "dllmain.h"

void Init()
{
	// Load the original dinput8.dll from the system directory
	char DInputDllName[MAX_PATH];
	GetSystemDirectoryA(DInputDllName, MAX_PATH);
	strcat_s(DInputDllName, "\\dinput8.dll");
	DInput8DLL = LoadLibraryA(DInputDllName);
	if (DInput8DLL > (HMODULE)31)
	{
		OriginalFunction = (DirectInput8Create_t)GetProcAddress(DInput8DLL, "DirectInput8Create");
	}

	InitializeHooking();
}

void Deinit()
{
	VirtualFree(CodeMem, 0, MEM_RELEASE);
}

BOOL APIENTRY DllMain(HMODULE Module,
	DWORD  ReasonForCall,
	LPVOID Reserved)
{
	switch (ReasonForCall)
	{
	case DLL_PROCESS_ATTACH:
		Init();
		break;
	case DLL_PROCESS_DETACH:
		Deinit();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

