#include "Viewmodel.h"

struct OffsetTable { const double Offsets_[3]; };

static std::map<uint32_t, OffsetTable*> OffsetWeaponTable = {};
static std::map<uint32_t, double> OffsetArmorTable = {};

void OffsetWeaponTableInsert(const uint32_t* IDTable, OffsetTable& Offsets, const size_t NumItems)
{
	for (int i = 0; i < NumItems; i++)
	{
		OffsetWeaponTable.insert({ IDTable[i] / 10000, &Offsets });
	}
}

void OffsetArmorTableInsert(const uint32_t* IDTable, const double Offset, const size_t NumItems)
{
	for (int i = 0; i < NumItems; i++)
	{
		OffsetArmorTable.insert({ IDTable[i] / 100000, Offset });
	}
}

const uint32_t overTheShoulderIDs[51] = { 2220000, 2240000, 2260000, 4060000, 6000000, 6020000,
	6040000, 6050000, 6070000, 6080000, 6100000, 6130000, 6140000, 6150000, 6170000,
	6180000, 6190000, 6200000, 6230000, 6250000, 6260000, 6270000, 6290000, 6300000,
	6310000, 6320000, 6330000, 6340000, 6360000, 6370000, 6380000, 7050000, 7110000,
	7120000, 7180000, 8060000, 8080000, 8110000, 8160000, 8170000, 8180000, 8190000,
	8250000, 8260000, 8270000, 8290000, 8300000, 8310000, 8320000, 16050000, 16160000 };
const uint32_t scytheIDs[4] = { 10000000, 10070000, 10080000, 10180000 };
const uint32_t FUGSID[1] = { 6350000 };

const double offsetScythes2h = 0.05;
const double offsetOTS1h = 0.0;
const double offsetWeaponNorm = 0.02;
const double offsetOTS2h = -0.04;
const double offsetFUGS1h = -0.05;
const double offsetFUGS2h = -0.09;

OffsetTable offsetsOTS = {offsetOTS1h, offsetOTS2h, offsetOTS2h};
OffsetTable offsetsScythes = { offsetWeaponNorm, offsetScythes2h, offsetScythes2h };
OffsetTable offsetsFUGS = { offsetFUGS1h, offsetFUGS2h, offsetFUGS2h };

const uint32_t smoughID[1] = { 94000000 };
const uint32_t smaclipIDs[2] = { 55000000, 92000000 };
const uint32_t mvmaclipIDs[3] = { 60500000, 75000000, 58000000 };
const uint32_t vmaclipIDs[2] = { 57000000, 85000000 };
const uint32_t maclipIDs[4] = { 28000000, 58500000, 66000000, 96000000 };
const uint32_t clipIDs[2] = { 32000000, 93000000 };
const uint32_t miclipIDs[3] = { 26500000, 82000000, 90000000 };
const uint32_t vmiclipIDs[4] = { 26000000, 55500000, 56000000, 72000000 };

const double offsetSmough = 0.093;
const double offsetSMaClip = 0.069;
const double offsetMVMaClip = 0.055;
const double offsetVMaClip = 0.053;
const double offsetMaClip = 0.045;
const double offsetClip = 0.043;
const double offsetMiClip = 0.033;
const double offsetVMiClip = 0.031;
const double offsetArmorNorm = 0.023;

extern void PopulateViewmodelTables()
{
	OffsetWeaponTableInsert(overTheShoulderIDs, offsetsOTS, 51);
	OffsetWeaponTableInsert(scytheIDs, offsetsScythes, 4);
	OffsetWeaponTableInsert(FUGSID, offsetsFUGS, 1);

	OffsetArmorTableInsert(smoughID, offsetSmough, 1);
	OffsetArmorTableInsert(smaclipIDs, offsetSMaClip, 2);
	OffsetArmorTableInsert(mvmaclipIDs, offsetMVMaClip, 3);
	OffsetArmorTableInsert(vmaclipIDs, offsetVMaClip, 2);
	OffsetArmorTableInsert(maclipIDs, offsetMaClip, 4);
	OffsetArmorTableInsert(clipIDs, offsetClip, 2);
	OffsetArmorTableInsert(miclipIDs, offsetVMiClip, 3);
	OffsetArmorTableInsert(vmiclipIDs, offsetVMiClip, 4);
}

extern double GetOffsetWeapon()
{
	const int32_t Handedness = *reinterpret_cast<uint32_t*>(TraversePtr(ArmStylePtr));
	if (!Handedness)
	{
		return offsetWeaponNorm;
	}

	const int32_t WeaponSlotRight = *reinterpret_cast<uint32_t*>(TraversePtr(ArmStylePtr, 0x4, 1)) * 2;
	const int32_t WeaponSlotLeft = *reinterpret_cast<uint32_t*>(TraversePtr(ArmStylePtr, 0x4, 2)) * 2;

	uint32_t EquippedWeaponID = 0;

	switch (Handedness)
	{
	case 0:
		return offsetWeaponNorm;
		break;
	case 2:
		EquippedWeaponID = *reinterpret_cast<uint32_t*>(TraversePtr(EquippedWeaponPtr, 0x4, WeaponSlotLeft)) / 10000;
		break;
	case 1:
	case 3:
		EquippedWeaponID = *reinterpret_cast<uint32_t*>(TraversePtr(EquippedWeaponPtr, 0x4, WeaponSlotRight + 1)) / 10000;
		break;
	}

	if (OffsetWeaponTable[EquippedWeaponID] == nullptr)
	{
		return offsetWeaponNorm;
	}
	else
	{
		return OffsetWeaponTable[EquippedWeaponID]->Offsets_[Handedness - 1];
	}
}

extern double GetOffsetArmor()
{
	uint32_t EquippedArmorID = *reinterpret_cast<uint32_t*>(TraversePtr(EquippedArmorPtr)) / 100000;

	if (OffsetArmorTable[EquippedArmorID] == 0)
	{
		return offsetArmorNorm;
	}

	return OffsetArmorTable[EquippedArmorID];
}