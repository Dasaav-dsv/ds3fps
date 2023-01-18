#include "FPS_ini.h"

void IniParseLineB(mINI::INIStructure& ini, const char* section, const char* key, bool& value)
{
	if (ini[section].has(key))
	{
		value = std::string("true").compare(ini[section][key]) == 0 ? true : false;
	}
	else
	{
		ini[section][key] = value ? "true" : "false";
	}
}

void IniParseLineD(mINI::INIStructure& ini, const char* section, const char* key, double& value)
{
	if (ini[section].has(key))
	{
		value = atof(ini[section][key].data());
	}
	else
	{
		ini[section][key] = std::to_string(value);
	}
}

void IniParseLineI(mINI::INIStructure& ini, const char* section, const char* key, int& value)
{
	if (ini[section].has(key))
	{
		value = atoi(ini[section][key].data());
	}
	else
	{
		ini[section][key] = std::to_string(value);
	}
}

extern void ParseFPSIni()
{
	mINI::INIFile file("FPS.ini");
	mINI::INIStructure ini;
	if (file.read(ini))
	{
		IniParseLineI(ini, "General", "Base FOV", fp_fov_base);

		IniParseLineB(ini, "Head Tracking", "Track Dodges", track_roll);
		IniParseLineB(ini, "Head Tracking", "Track on Hit", track_hit);

		IniParseLineB(ini, "Debug", "Debug Console Output", test_out);
		IniParseLineB(ini, "Debug", "Use VirtualAlloc2", use_virtualalloc2);

		file.write(ini, true);
	}
	else
	{
		ini["General"]["Base FOV"] = std::to_string(fp_fov_base);

		ini["Head Tracking"]["Track Dodges"] = track_roll ? "true" : "false";
		ini["Head Tracking"]["Track on Hit"] = track_hit ? "true" : "false";
		
		ini["Debug"]["Debug Console Output"] = test_out ? "true" : "false";
		ini["Debug"]["Use VirtualAlloc2"] = use_virtualalloc2 ? "true" : "false";

		file.generate(ini, true);
	}
}
