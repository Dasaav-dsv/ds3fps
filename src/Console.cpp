#include "Console.h"

static volatile bool AllocState = false;

extern void __declspec(noinline) AllocConsoleOnce()
{
	if (AllocState)
	{
		return;
	}

	if (AllocConsole())
	{
		FILE* stream;
		AllocState = !freopen_s(&stream, "CON", "w", stdout);
	}
}
