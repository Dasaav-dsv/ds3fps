#pragma once

#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable : 6387 26451 26495 26498)
#include "../include/asmjit/asmjit.h"
#include "../include/MCF/VirtualAllocNear.hpp"
#pragma warning(pop)

#include "Console.h"
#include "Trampoline.h"
#include "MsgRepoPatch.h"
#include "camera/Camera.h"

const static void* pInitializeCamera = &InitializeCamera;
const static void* pDeinitializeCamera = &DeinitializeCamera;
const static void* pPerspectiveSwitch = &PerspectiveSwitch;
const static void* pGenerateInputPointers = &GenerateInputPointers;
const static void* pGetTAEData = &GetTAEData;
const static void* pAttachToHead = &AttachToHead;
const static void* pPatchMsgRepo = &PatchMsgRepo;

extern void InjectAsm();