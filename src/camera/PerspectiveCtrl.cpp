#include "PerspectiveCtrl.h"

extern void CameraFPS()
{
	if (!CamInit)
	{
		return;
	}

	PDATA SprjChrPhysicsModulePtr[] = {&WorldChrMan, 0x40, 0x28};

	if (reinterpret_cast<void*>(TraversePtr(SprjChrPhysicsModulePtr)) != nullptr)
	{
		ViewMatrix* cam_mtx = reinterpret_cast<ViewMatrix*>(TraversePtr(DebugCamPtr));
		ViewMatrix* rend_mtx = reinterpret_cast<ViewMatrix*>(TraversePtr(BaseCamPtr));

		const double PRot = *reinterpret_cast<float*>(TraversePtr(PlayerRotPtr));
		const double sinPRot = sin(PRot);
		const double cosPRot = cos(PRot);

		V4D2 var = V4D2(-cosPRot, 0, sinPRot, 0);
		V4D2 vbr = V4D2(0, 1, 0, 0);
		V4D2 vcr = V4D2(-sinPRot, 0, -cosPRot, 0);

		VxD2::write_vmtx(var, vbr, vcr, cam_mtx);
		VxD2::write_vmtx(var, vbr, vcr, rend_mtx);
	}
	else
	{
		ViewMatrix* cam_mtx = reinterpret_cast<ViewMatrix*>(TraversePtr(DebugCamPtr));
		ViewMatrix* rend_mtx = reinterpret_cast<ViewMatrix*>(TraversePtr(BaseCamPtr));

		VxD::write_umtx(cam_mtx);
		VxD::write_umtx(rend_mtx);
	}
	SetEventFlag(*reinterpret_cast<void**>(SprjEventFlagMan), 1099, true, 0);
	*TraversePtr(LbStrafePtr) = 0;

	*reinterpret_cast<float*>(TraversePtr(FOVZoom2Ptr)) = RADf(125.0f);

	memcpy(TraversePtr(UpperArmParamPtr), UpperArmParamData, 80);

	IsFPS = true;
}

static float PressTime = 0.0f;
static bool PressState = false;

extern void Camera3PS()
{
	if (!CamInit)
	{
		return;
	}

	SetEventFlag(*reinterpret_cast<void**>(SprjEventFlagMan), 1099, false, 0);

	*TraversePtr(CamModePtr) = 0;
	*TraversePtr(LbStrafePtr) = 1;

	ToggleRunFPS = false;

	*reinterpret_cast<float*>(TraversePtr(FOVPtr)) = RADf(120.0f);
	*reinterpret_cast<float*>(TraversePtr(FOV2Ptr)) = RADf(43.0f);
	*reinterpret_cast<float*>(TraversePtr(FOVZoom2Ptr)) = RADf(25.0f);

	std::memset(TraversePtr(UpperArmParamPtr), 0, 80);

	IsFPS = false;
}

extern void PerspectiveSwitch()
{
	if (!CamInit) 
	{ 
		return; 
	}

	float _dT = *reinterpret_cast<float*>(TraversePtr(DtPtr));
	dT.exchange(_dT);

	if (*TraversePtr(LockOnPressStateUniversalPtr, *reinterpret_cast<uint32_t*>(TraversePtr(LockOnPressStateUniversalOffsetPtr)) * 4) == 0 && *TraversePtr(MenuStatePtr) == 0)
	{
		PressTime += _dT;
		if (!(IsFPS || PressState))
		{
			if (PressTime > 0.2f)
			{
				*TraversePtr(LockModeDropPtr) = 1;
			}
			if (PressTime > 0.3f && *TraversePtr(LockMode1Ptr) == 0 && *TraversePtr(LockMode2Ptr) == 0)
			{
				PressState = true;
				CameraFPS();
				return;
			}
		}
		else if (IsFPS && !PressState)
		{
			if (PressTime > 0.3f)
			{
				PressState = true;
				Camera3PS();
				return;
			}
		}
	}
	else
	{
		PressTime = 0.0f;
		PressState = false;
	}
}
