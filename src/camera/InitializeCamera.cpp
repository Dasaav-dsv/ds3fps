#include "InitializeCamera.h"

static int BoneOffsetData1[18] = {};
static int BoneOffsetData2[18] = {};

extern void InitializeCamera()
{
	if (reinterpret_cast<uint64_t*>(WorldChrMan) != nullptr)
	{
		for (int i = 0; i < 18; i++)
		{
			uint32_t BoneOffset = 0x90 * *reinterpret_cast<uint32_t*>(TraversePtr(BoneOffsetPtr, 0x4, i));

			BoneOffsetData1[i] = BoneOffset;
			BoneOffset += 0x68;

			const PDATA pBoneOffset[] = {&FrpgHavokMan, 0x38, 0x8, 0x20, static_cast<PDATA>(BoneOffset)};

			BoneOffsetData2[i] = 0x80 * *reinterpret_cast<uint32_t*>(TraversePtr(pBoneOffset));
		}

		HeadBonePosPtr[4] = BoneOffsetData2[14];
		HeadBoneQPtr[4] = BoneOffsetData2[14] + 0x10;
		CamInit = true;

		if (GetEventFlag(*reinterpret_cast<void**>(SprjEventFlagMan), 1099))
		{
			IsFPS = true;
		}

		if (IsFPS)
		{
			CameraFPS();
		}
		else
		{
			Camera3PS();
		}
	}
}

extern void DeinitializeCamera()
{
	CamInit = false;
}