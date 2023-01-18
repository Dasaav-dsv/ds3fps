#include "EventFlag.h"

extern decltype(&::DefGetEventFlag) GetEventFlag = nullptr;

extern decltype(&::DefSetEventFlag) SetEventFlag = nullptr;

void GetEventFunctions(const char* pGetFlag, const char* pSetFlag)
{
	GetEventFlag = reinterpret_cast<decltype(&::DefGetEventFlag)>(pGetFlag);

	SetEventFlag = reinterpret_cast<decltype(&::DefSetEventFlag)>(pSetFlag);
}
