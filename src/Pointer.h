#pragma once

#include "stdafx.h"

class PDATA
{
public:
	union
	{
		int64_t Base_ = 0;
		int32_t Offset_;
	}Data_;

	PDATA() {}

	PDATA(void** Base)
	{
		this->Data_.Base_ = reinterpret_cast<int64_t>(Base);
	}

	PDATA(void*** pBase)
	{
		this->Data_.Base_ = -reinterpret_cast<int64_t>(pBase);
	}

	PDATA(char** Base)
	{
		this->Data_.Base_ = reinterpret_cast<int64_t>(Base);
	}

	PDATA(char*** pBase)
	{
		this->Data_.Base_ = -reinterpret_cast<int64_t>(pBase);
	}

	PDATA(const int32_t Offset)
	{
		this->Data_.Offset_ = Offset;
	}

	~PDATA() {}

	friend char* operator + (char*& Base, const PDATA& Offset)
	{
		return Base + Offset.Data_.Offset_;
	}

	char* operator* () const
	{
		if (this->Data_.Base_ > 0)
		{
			return *reinterpret_cast<char**>(this->Data_.Base_);
		}
		else
		{
			return **reinterpret_cast<char***>(-this->Data_.Base_);
		}
	}

	operator int()
	{
		return this->Data_.Offset_;
	}
};

template <size_t N> inline char* TraversePtr(const PDATA(&Ptr)[N], const int32_t AddOffset = 0, const int32_t NumAdd = 1)
{
	char* p = *Ptr[0];

	if (N == 1)
	{
		return p;
	};

	p = p + Ptr[1];

	if (N == 2)
	{
		return p;
	};

	for (int i = 2; i < N - 1; i++)
	{
		p = reinterpret_cast<char*>(*reinterpret_cast<uint64_t*>(p));
		p = p + Ptr[i];
	}

	p = reinterpret_cast<char*>(*reinterpret_cast<uint64_t*>(p));
	p = p + Ptr[N - 1] + AddOffset * NumAdd;

	return p;
}

extern void* CodeMem;

extern char** GameDataMan;
extern char** WorldChrMan;
extern char** FieldArea;
extern char** LockTgtMan;
extern char** SprjFlipper;
extern char** SprjRemo;
extern char** SprjEventFlagMan;
extern char** UserInputManager;
extern char** FrpgHavokMan;
extern char** SoloParamRepository;
extern char** MsgRepository;
extern char** NewMenuSystem;
extern char** CSPcKeyConfig;
extern int* CSLang;

extern char* MagicPtr;
extern char* FrameCounter;
extern char* BoneOffsetBasePointer;
extern char* InputPtrBase;

extern char* TAE_PtrBase;
extern char* TAE_PtrOffset;
extern char* TAE_PtrNameOffset;

extern char* LockOnCodeAOB;
extern char* LOC_CallAOB;
extern char* BoneOffsetBasePointerAOB;
extern char* BOBP_CallAOB;
extern char* CameraRendPointerAOB;
extern char* CallSaveLoadEndAOB;
extern char* CSLE_CallAOB;
extern char* DtorStartAOB;
extern char* StrafeCheckAOB1;
extern char* StrafeCheckAOB2;
extern char* GetTAEAOB;
extern char* GenInputPtrAOB;
extern char* GenMsgCodeAOB;
extern char* FallDeathCodeAOB;
extern char* HeadSizeCodeAOB;
extern char* UpperHandCodeAOB;
extern char* CamSensCodeAOB;
extern char* MenuValCodeAOB;
extern char* StartScreenHookAOB;
extern char* HkRunCodeAOB;
extern char* RunStateCodeAOB;
extern char* AimFadeCodeAOB;

extern char* CamCodeAOB01;
extern char* CamCodeAOB02;
extern char* CamCodeAOB03;
extern char* CamCodeAOB04;
extern char* CamCodeAOB05;
extern char* CamCodeAOB06;
extern char* CamCodeAOB07;
extern char* CamCodeAOB08;
extern char* CamCodeAOB09;
extern char* CamCodeAOB10;
extern char* CamCodeAOB11;
extern char* CamCodeAOB12;
extern char* CamCodeAOB13;
extern char* CamCodeAOB14;
extern char* CamCodeAOB15;
extern char* CamCodeAOB16;
extern char* CamCodeAOB17;
extern char* CamCodeAOB18;
extern char* CamCodeAOB19;
extern char* CamCodeAOB20;
extern char* CamCodeAOB21;
extern char* CamCodeAOB22;
extern char* CamCodeAOB23;
extern char* CamCodeAOB24;
extern char* CamCodeAOB25;
extern char* CamCodeAOB26;
extern char* CamCodeAOB27;
extern char* CamCodeAOB28;
extern char* CamCodeAOB29;
extern char* CamCodeAOB30;

extern char* GetEventFlagAOB;
extern char* SetEventFlagAOB;

extern PDATA BaseCamPtr[4];
extern PDATA FollowCamPtr[4];
extern PDATA RendCamPtr[3];
extern PDATA DebugCamPtr[4];
extern PDATA AimCamPtr[4];
extern PDATA AimCamBasePtr[4];
extern PDATA CamModePtr[3];
extern PDATA AimCamModePtr[3];
extern PDATA TrackDodgesPtr[4];

extern PDATA InvertPad[3];
extern PDATA InvertMouse[2];

extern PDATA FOVPtr[4];
extern PDATA FOV2Ptr[4];
extern PDATA FOVZoomPtr[4];
extern PDATA FOVZoom2Ptr[4];

extern PDATA PlayerPosPtr[4];
extern PDATA PlayerRotPtr[4];

extern PDATA LbStrafePtr[4];
extern PDATA REMOStatePtr[3];

extern PDATA EquippedWeaponPtr[3];
extern PDATA EquippedArmorPtr[3];
extern PDATA ArmStylePtr[3];

extern PDATA AnimIDPtr[5];
extern PDATA AnimTimePtr[5];
extern PDATA DtPtr[2];

extern PDATA LockMode1Ptr[2];
extern PDATA LockMode2Ptr[2];
extern PDATA LockModeDropPtr[2];

extern PDATA MenuStatePtr[4];
extern PDATA ObjActStatePtr[4];
extern PDATA RunPressStatePtr[4];
extern PDATA RunPressTimePtr[4];

extern PDATA LockOnPressStatePadPtr[20];
extern PDATA LockOnPressStateMousePtr[7];
extern PDATA LockOnPressStateUniversalPtr[15];
extern PDATA LockOnPressStateUniversalOffsetPtr[17]; // alt: 12

extern PDATA InteractPressStateUniversalPtr[15];
extern PDATA InteractPressStateUniversalOffsetPtr[17];

extern PDATA BoneOffsetPtr[7];

extern PDATA MsgRepo1Ptr[5];
extern PDATA MsgRepo2Ptr[5];
extern PDATA MsgRepo3Ptr[5];

extern PDATA HeadBonePosPtr[5];
extern PDATA HeadBoneQPtr[5];

extern PDATA MouseInputLRPtr[5];
extern PDATA MouseInputUDPtr[5];
extern PDATA MouseSensPtr[2];

extern PDATA RStickInputLRPtr[5];
extern PDATA RStickInputUDPtr[5];
extern PDATA PadSensPtr[4];

extern PDATA MovementInputLRPtr[5];
extern PDATA MovementInputUDPtr[5];
extern PDATA HkbRunStatePtr[10];

extern PDATA UpperArmParamPtr[5];