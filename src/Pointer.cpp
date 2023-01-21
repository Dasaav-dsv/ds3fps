#include "Pointer.h"

extern void* CodeMem = nullptr;

extern char** GameDataMan = nullptr;
extern char** WorldChrMan = nullptr;
extern char** FieldArea = nullptr;
extern char** LockTgtMan = nullptr;
extern char** SprjFlipper = nullptr;
extern char** SprjRemo = nullptr;
extern char** SprjEventFlagMan = nullptr;
extern char** UserInputManager = nullptr;
extern char** FrpgHavokMan = nullptr;
extern char** SoloParamRepository = nullptr;
extern char** MsgRepository = nullptr;
extern char** NewMenuSystem = nullptr;
extern char** CSPcKeyConfig = nullptr;
extern int* CSLang = nullptr;

extern char* MagicPtr = nullptr;
extern char* FrameCounter = nullptr;
extern char* BoneOffsetBasePointer = nullptr;
extern char* InputPtrBase = nullptr;

extern char* TAE_PtrBase = nullptr;
extern char* TAE_PtrOffset = nullptr;
extern char* TAE_PtrNameOffset = nullptr;

extern char* LockOnCodeAOB = nullptr;
extern char* LOC_CallAOB = nullptr;
extern char* BoneOffsetBasePointerAOB = nullptr;
extern char* BOBP_CallAOB = nullptr;
extern char* CameraRendPointerAOB = nullptr;
extern char* CallSaveLoadEndAOB = nullptr;
extern char* CSLE_CallAOB = nullptr;
extern char* DtorStartAOB = nullptr;
extern char* StrafeCheckAOB1 = nullptr;
extern char* StrafeCheckAOB2 = nullptr;
extern char* GetTAEAOB = nullptr;
extern char* GenInputPtrAOB = nullptr;
extern char* GenMsgCodeAOB = nullptr;
extern char* FallDeathCodeAOB = nullptr;
extern char* HeadSizeCodeAOB = nullptr;
extern char* UpperHandCodeAOB = nullptr;
extern char* CamSensCodeAOB = nullptr;
extern char* MenuValCodeAOB = nullptr;
extern char* StartScreenHookAOB = nullptr;
extern char* HkRunCodeAOB = nullptr;
extern char* RunStateCodeAOB = nullptr;
extern char* AimFadeCodeAOB = nullptr;

extern char* CamCodeAOB01 = nullptr;
extern char* CamCodeAOB02 = nullptr;
extern char* CamCodeAOB03 = nullptr;
extern char* CamCodeAOB04 = nullptr;
extern char* CamCodeAOB05 = nullptr;
extern char* CamCodeAOB06 = nullptr;
extern char* CamCodeAOB07 = nullptr;
extern char* CamCodeAOB08 = nullptr;
extern char* CamCodeAOB09 = nullptr;
extern char* CamCodeAOB10 = nullptr;
extern char* CamCodeAOB11 = nullptr;
extern char* CamCodeAOB12 = nullptr;
extern char* CamCodeAOB13 = nullptr;
extern char* CamCodeAOB14 = nullptr;
extern char* CamCodeAOB15 = nullptr;
extern char* CamCodeAOB16 = nullptr;
extern char* CamCodeAOB17 = nullptr;
extern char* CamCodeAOB18 = nullptr;
extern char* CamCodeAOB19 = nullptr;
extern char* CamCodeAOB20 = nullptr;
extern char* CamCodeAOB21 = nullptr;
extern char* CamCodeAOB22 = nullptr;
extern char* CamCodeAOB23 = nullptr;
extern char* CamCodeAOB24 = nullptr;
extern char* CamCodeAOB25 = nullptr;
extern char* CamCodeAOB26 = nullptr;
extern char* CamCodeAOB27 = nullptr;
extern char* CamCodeAOB28 = nullptr;
extern char* CamCodeAOB29 = nullptr;
extern char* CamCodeAOB30 = nullptr;

extern char* GetEventFlagAOB = nullptr;
extern char* SetEventFlagAOB = nullptr;

extern PDATA BaseCamPtr[] = { &FieldArea, 0x28, 0x60, 0x10 };
extern PDATA FollowCamPtr[] = { &FieldArea, 0x28, 0x60, 0x60 };
extern PDATA RendCamPtr[] = { &FieldArea, 0x28, 0x10 };
extern PDATA DebugCamPtr[] = { &FieldArea, 0x18, 0xE8, 0x10 };
extern PDATA AimCamPtr[] = { &FieldArea, 0x28, 0x68, 0x10 };
extern PDATA AimCamBasePtr[] = { &FieldArea, 0x28, 0x68, 0x90 };
extern PDATA CamModePtr[] = { &FieldArea, 0x18, 0xE0 };
extern PDATA AimCamModePtr[] = { &FieldArea, 0x28, 0x84 };
extern PDATA TrackDodgesPtr[] = { &FieldArea, 0x28, 0x60, 0x2D2 };

extern PDATA FOVPtr[] = { &FieldArea, 0x18, 0xE8, 0x50 };
extern PDATA FOV2Ptr[] = { &FieldArea, 0x28, 0x60, 0x50 };
extern PDATA FOVZoomPtr[] = { &FieldArea, 0x28, 0x68, 0x50 };
extern PDATA FOVZoom2Ptr[] = { &FieldArea, 0x28, 0x68, 0x134 };

extern PDATA PlayerPosPtr[] = { &WorldChrMan, 0x40, 0x28, 0x80 };
extern PDATA PlayerRotPtr[] = { &WorldChrMan, 0x40, 0x28, 0x74 };

extern PDATA LbStrafePtr[] = { &WorldChrMan, 0x80, 0x50, 0x2C1 };
extern PDATA REMOStatePtr[] = { &SprjRemo, 0x8, 0x48 };

extern PDATA EquippedWeaponPtr[] = { &GameDataMan, 0x10, 0x32C };
extern PDATA EquippedArmorPtr[] = { &GameDataMan, 0x10, 0x360 };
extern PDATA ArmStylePtr[] = { &GameDataMan, 0x10, 0x2B8 };

extern PDATA AnimIDPtr[] = { &WorldChrMan, 0x80, 0x1F90, 0x80, 0xC8 };
extern PDATA AnimTimePtr[] = { &WorldChrMan, 0x80, 0x1F90, 0x10, 0x24 };
extern PDATA DtPtr[] = { &SprjFlipper, 0x268 };

extern PDATA LockMode1Ptr[] = { &LockTgtMan, 0x2820 };
extern PDATA LockMode2Ptr[] = { &LockTgtMan, 0x2821 };
extern PDATA LockModeDropPtr[] = { &LockTgtMan, 0x2822 };

extern PDATA MenuStatePtr[] = { &WorldChrMan, 0x80, 0x58, 0x418 };
extern PDATA ObjActStatePtr[] = { &WorldChrMan, 0x80, 0x50, 0x183 };
extern PDATA RunPressStatePtr[] = { &WorldChrMan, 0x80, 0x58, 0x411 };
extern PDATA RunPressTimePtr[] = { &WorldChrMan, 0x80, 0x58, 0x348 };

extern PDATA LockOnPressStatePadPtr[] = { &UserInputManager, 0x38, 0x0, 0x8, 0x28, 0xA8, 0x0, 0x8, 0x10, 0x8, 0x8, 0x10, 0x0, 0x0, 0x8, 0x10, 0x8, 0x8, 0x10, 0x20 };
extern PDATA LockOnPressStateMousePtr[] = { &UserInputManager, 0x38, 0x0, 0x0, 0x28, 0x80, 0x1A };
extern PDATA LockOnPressStateUniversalPtr[] = { &UserInputManager, 0x18, 0x8, 0x120, 0x8, 0x10, 0x0, 0x8, 0x0, 0x0, 0x8, 0x0, 0x38, 0x10, 0x0 };
extern PDATA LockOnPressStateUniversalOffsetPtr[] = { &UserInputManager, 0x18, 0x8, 0x120, 0x8, 0x10, 0x0, 0x8, 0x0, 0x0, 0x0, 0x8, 0x8, 0x48, 0x38, 0x8, 0x54}; // alt: { &UserInputManager, 0x38, 0x0, 0x8, 0x0, 0x28, 0x28, 0x38, 0x0, 0x8, 0x0, 0x20 };

extern PDATA InteractPressStateUniversalPtr[] = { &UserInputManager, 0x18, 0x8, 0x120, 0x8, 0x10, 0x0, 0x8, 0x0, 0x0, 0x8, 0x0, 0x38, 0x10, 0x0 };
extern PDATA InteractPressStateUniversalOffsetPtr[] = { &UserInputManager, 0x18, 0x8, 0x120, 0x8, 0x10, 0x0, 0x8, 0x0, 0x0, 0x0, 0x8, 0x8, 0x48, 0x38, 0x8, 0x4 };

extern PDATA BoneOffsetPtr[] = { &WorldChrMan, 0x80, 0x50, 0x20, 0x38, 0x20, 0x0 }; // -> PlayerIns -> PlayerCtrl -> RagdollIns -> hknpRagdoll ->

extern PDATA MsgRepo1Ptr[] = { &MsgRepository, 0x8, 0x0, 0x640, 0x0 };
extern PDATA MsgRepo2Ptr[] = { &MsgRepository, 0x8, 0x0, 0x648, 0x0 };
extern PDATA MsgRepo3Ptr[] = { &MsgRepository, 0x8, 0x0, 0x658, 0x0 };

extern PDATA HeadBonePosPtr[] = { &FrpgHavokMan, 0x38, 0x8, 0xE0, 0x0 }; // -> FrpgPhysWorld -> hknpWorld ->
extern PDATA HeadBoneQPtr[] = { &FrpgHavokMan, 0x38, 0x8, 0xE0, 0x0 };

extern PDATA MouseInputLRPtr[] = { &UserInputManager, 0x18, 0x8, 0x150, 0x0 }; // -> DLUID::VirtualMultiDevice<DLKR::DLMultiThreadingPolicy> -> 
extern PDATA MouseInputUDPtr[] = { &UserInputManager, 0x18, 0x8, 0x150, 0x0 };
extern PDATA MouseSensPtr[] = { &CSPcKeyConfig, 0x710 };

extern PDATA RStickInputLRPtr[] = { &UserInputManager, 0x18, 0x8, 0x150, 0x0 };
extern PDATA RStickInputUDPtr[] = { &UserInputManager, 0x18, 0x8, 0x150, 0x0 };
extern PDATA PadSensPtr[] = { &FieldArea, 0x28, 0x60, 0x280 };

extern PDATA MovementInputLRPtr[] = { &UserInputManager, 0x18, 0x8, 0x150, 0x0 };
extern PDATA MovementInputUDPtr[] = { &UserInputManager, 0x18, 0x8, 0x150, 0x0 };
extern PDATA HkbRunStatePtr[] = { &WorldChrMan, 0x80, 0x1F90, 0x28, 0x60, 0x28, 0x80, 0x110, 0x10, 0x140 };

extern PDATA UpperArmParamPtr[] = { &SoloParamRepository, 0x1618, 0x68, 0x68, 0xD0 };