#include "Hook.h"

PROCESS_BASIC_INFORMATION BasicProcessInfo;

void InitializeHooking()
{
	HANDLE pHandle = GetCurrentProcess();
	GetModuleInformation(pHandle, GetModuleHandleA("DarkSoulsIII.exe"), &mInfo, sizeof(mInfo));

	DWORD OldProtect = PAGE_EXECUTE_READ;
	VirtualProtect(mInfo.lpBaseOfDll, mInfo.SizeOfImage, PAGE_EXECUTE_READWRITE, &OldProtect);

	ParseFPSIni();

	if (test_out)
	{
		AllocConsoleOnce();
	}

	GetText();

	PopulateBases();

	ScanAOBs();

	GetEventFunctions(GetEventFlagAOB, SetEventFlagAOB);

	InjectAsm();

	PopulateViewmodelTables();
}

