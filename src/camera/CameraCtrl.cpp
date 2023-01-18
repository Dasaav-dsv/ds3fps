#include "CameraCtrl.h"

/*
	a - left
	b - top
	c - forward

	v_r - reference
	v__ - _(1) component of _(2)
	v_o - camera direction (original)
	v_  - camera direction (new)
	v_2 - character direction (new)
*/

#define fade(t) ( sin(4 * t + M_PI / 16.0) )

static V4D2 Q_Prev = umtx.r[3];
static volatile bool RunStateB = false;
static volatile float RunTimeout = 0.0f;

extern void AttachToHead()
{
	if (!CamInit)
	{
		return;
	}

	if (*TraversePtr(REMOStatePtr) > 3)
	{
		*TraversePtr(CamModePtr) = 0;
		return;
	}

	*TraversePtr(CamModePtr) = 3;

	const bool isEngLang = *CSLang == 8;

	float* FOV = reinterpret_cast<float*>(TraversePtr(FOVPtr));
	float* FOV2 = reinterpret_cast<float*>(TraversePtr(FOV2Ptr));
	float* FOV_zoom = reinterpret_cast<float*>(TraversePtr(FOVZoomPtr));

	const float FOV_base = (20.0f * (*reinterpret_cast<float*>(TraversePtr(PadSensPtr)) - 0.5f) + static_cast<float>(fp_fov_base)) * M_PI_180f;
	float _FOV = *FOV;

	if (*TraversePtr(AimCamModePtr) == 0)
	{
		if (isEngLang)
		{
			_FOV = _FOV * 0.9f + FOV_base * 0.1f;
		}
		else
		{
			_FOV = static_cast<float>(fp_fov_base);
		}
	}
	else
	{
		_FOV = _FOV * 0.75f + *FOV_zoom * 0.09f;
	}

	*FOV = _FOV;
	*FOV2 = _FOV;

	ViewMatrix* dcam_mtx = reinterpret_cast<ViewMatrix*>(TraversePtr(DebugCamPtr));
	ViewMatrix* base_mtx = reinterpret_cast<ViewMatrix*>(TraversePtr(BaseCamPtr));
	ViewMatrix* follow_mtx = reinterpret_cast<ViewMatrix*>(TraversePtr(FollowCamPtr));
	ViewMatrix* aim_mtx = reinterpret_cast<ViewMatrix*>(TraversePtr(AimCamPtr));

	const V4D2 Q = V4D2(reinterpret_cast<float*>(TraversePtr(HeadBoneQPtr)));

	const V4D2 Q_Transform = Q.qDiv(Q_Prev).normalize();

	float* pPRot = reinterpret_cast<float*>(TraversePtr(PlayerRotPtr));
	float PRot = *pPRot;
	double cosPRot = cos(PRot);
	double sinPRot = sin(PRot);

	const bool RunPressState = *reinterpret_cast<float*>(TraversePtr(RunPressTimePtr)) > 0.0f;
	bool RunState = *TraversePtr(HkbRunStatePtr) == 2;
	RunState = RunState && RunStateB ? true : RunState && !RunStateB;

	const bool ObjActState = *TraversePtr(ObjActStatePtr) != 0;

	if (RunTimeout > 0.0f)
	{
		RunTimeout -= dT.load();
	}

	if ((!RunState && RunPressState) || (!RunPressState && RunState))
	{
		RunTimeout = 0.5f;
	}

	*TraversePtr(LbStrafePtr) = RunState;
	ToggleRunFPS = 1;

	V4D2 var = V4D2(-cosPRot, 0, sinPRot, 0);
	V4D2 vbr = V4D2(0, 1, 0, 0);
	V4D2 vcr = V4D2(-sinPRot, 0, -cosPRot, 0);

	const bool TrackHeadLocal = TrackHead;

	if (TrackHeadLocal && !RunState)
	{
		const double q0 = Q[0];
		const double q1 = Q[1];
		const double q2 = Q[2];
		const double q3 = Q[3];
		var = { 2 * q3 * q2 - 2 * q0 * q1, 2 * q0 * q0 + 2 * q2 * q2 - 1, -2 * q2 * q1 - 2 * q3 * q0 };
		vbr = { 1 - 2 * q2 * q2 - 2 * q1 * q1, 2 * q0 * q1 + 2 * q3 * q2, 2 * q0 * q2 - 2 * q3 * q1 };
		vcr = { 2 * q0 * q2 + 2 * q3 * q1, 2 * q2 * q1 - 2 * q3 * q0, 1 - 2 * q0 * q0 - 2 * q1 * q1 };
	}

	V4D2 vco = dcam_mtx->mtx[2];
	V4D2 vao = vbr.cross(vco).normalize();

	const V4D2 vbo = vco.cross(var).normalize();

	const V4D2 vcob = VxD::component2V(vco, vbr);
	const double vcoPL = VxD2::length(vco - vcob);

	double MouseInputLR;
	double MouseInputUD;

	float MaxRotationHdegs = MaxRotation.load();
	double sens_coeff = tanf(_FOV * 0.5f) / tanf(M_PIf * 0.25f);

	double sens_cam = 1.0;
	double sens_mouse = 1.0;
	double sens_pad = 1.0;

	const PDATA RStickOffsetLRPtr[] = { UserInputManager, 0x18, 0x8, 0x120, 0x0, 0x10, 0x0, 0x8, 0x0, 0x0, 0x0, 0x8, 0x0, 0x10, 0x20, 0x8 };
	const PDATA RStickOffsetUDPtr[] = { UserInputManager, 0x18, 0x8, 0x120, 0x0, 0x10, 0x0, 0x8, 0x0, 0x0, 0x0, 0x8, 0x0, 0x10, 0x28, 0x8 };

	const PDATA MouseOffsetLRPtr[] = { UserInputManager, 0x18, 0x8, 0x120, 0x8, 0x10, 0x0, 0x8, 0x0, 0x0, 0x0, 0x8, 0x0, 0x10, 0x20, 0x8 };
	const PDATA MouseOffsetUDPtr[] = { UserInputManager, 0x18, 0x8, 0x120, 0x8, 0x10, 0x0, 0x8, 0x0, 0x0, 0x0, 0x8, 0x0, 0x10, 0x28, 0x8 };

	const PDATA MovementOffsetLRPtr[] = { UserInputManager, 0x18, 0x8, 0x120, 0x8, 0x10, 0x0, 0x8, 0x0, 0x0, 0x0, 0x8, 0x0, 0x10, 0x0, 0x8 };
	const PDATA MovementOffsetUDPtr[] = { UserInputManager, 0x18, 0x8, 0x120, 0x8, 0x10, 0x0, 0x8, 0x0, 0x0, 0x0, 0x8, 0x0, 0x10, 0x8, 0x8 };

	RStickInputLRPtr[4] = *reinterpret_cast<int32_t*>(TraversePtr(RStickOffsetLRPtr)) * 4;
	RStickInputUDPtr[4] = *reinterpret_cast<int32_t*>(TraversePtr(RStickOffsetUDPtr)) * 4;

	MouseInputLRPtr[4] = *reinterpret_cast<int32_t*>(TraversePtr(MouseOffsetLRPtr)) * 4;
	MouseInputUDPtr[4] = *reinterpret_cast<int32_t*>(TraversePtr(MouseOffsetUDPtr)) * 4;

	MovementInputLRPtr[4] = *reinterpret_cast<int32_t*>(TraversePtr(MovementOffsetLRPtr)) * 4;
	MovementInputUDPtr[4] = *reinterpret_cast<int32_t*>(TraversePtr(MovementOffsetUDPtr)) * 4;

	if (isEngLang)
	{
		sens_cam = static_cast<double>(*reinterpret_cast<float*>(TraversePtr(MouseSensPtr))) * 0.1 + 0.5;
	}
	else
	{
		sens_mouse = static_cast<double>(*reinterpret_cast<float*>(TraversePtr(MouseSensPtr))) * 0.1 + 0.5;
		sens_pad = static_cast<double>(*reinterpret_cast<float*>(TraversePtr(PadSensPtr))) + 0.5;
	}

	if (MaxRotationHdegs != INFINITY)
	{
		sens_coeff *= 1 - 150 / (MaxRotationHdegs + 360);
	}

	if (*TraversePtr(MenuStatePtr) == 0)
	{
		MouseInputLR = static_cast<double>(*reinterpret_cast<float*>(TraversePtr(MouseInputLRPtr))) * vcoPL * 0.0010 * sens_coeff * sens_mouse * sens_cam;
		MouseInputUD = static_cast<double>(*reinterpret_cast<float*>(TraversePtr(MouseInputUDPtr))) * vcoPL * 0.0013 * sens_coeff * sens_mouse * sens_cam;
	}	
	else
	{
		MouseInputLR = 0;
		MouseInputUD = 0;
	}

	const double RStickInputLR = static_cast<double>(*reinterpret_cast<float*>(TraversePtr(RStickInputLRPtr))) * vcoPL * 0.075 * sens_coeff * sens_pad * sens_cam;
	const double RStickInputUD = static_cast<double>(*reinterpret_cast<float*>(TraversePtr(RStickInputUDPtr))) * vcoPL * 0.100 * sens_coeff * sens_pad * sens_cam;

	const double MovementInputLR = static_cast<double>(*reinterpret_cast<float*>(TraversePtr(MovementInputLRPtr)));
	const double MovementInputUD = static_cast<double>(*reinterpret_cast<float*>(TraversePtr(MovementInputUDPtr)));
	V4D2 MovementInput = { MovementInputUD, 0, MovementInputLR };

	const double vRot = tan(MouseInputUD - RStickInputUD);

	const V4D2 qV = V4D2(vao, vRot);

	V4D2 vc = vco + vao * (MouseInputLR + RStickInputLR);
	vc.normalize().qTransform(qV);
	V4D2 va = vbr.cross(vc).normalize();
	V4D2 vb = vc.cross(va).normalize();

	const bool IsLockHeld = *TraversePtr(LockOnPressStateMousePtr) == 1 || *TraversePtr(LockOnPressStatePadPtr) == 1;

	if (*TraversePtr(MenuStatePtr) == 0 && IsLockHeld)
	{
		MaxRotationHdegs = 0;
	}

	if (TrackHeadLocal)
	{
		vc.qTransform(Q_Transform);
		va = vbr.cross(vc);
		va.normalize();
	}

	if (RunPressState)
	{
		if (MovementInput.length() > 0.37)
		{
			V4D2 MILocal = MovementInput;
			MILocal.normalize();

			V4D2 MILocalV = va * MILocal[0] + vc * MILocal[2];
			MILocalV[1] = 0.0;
			MILocalV.normalize();

			const double relAng = -atan2(va[0] * MILocalV[2] - va[2] * MILocalV[0], va[0] * MILocalV[0] + va[2] * MILocalV[2]);
			const double maxRelAng = RAD(105.0);

			if (std::fabs(relAng) > maxRelAng)
			{
				RunState = false || RunStateB;
				ToggleRunFPS = 1;
				RunTimeout = 0.5f;
			}
			else
			{
				RunState = true;
				ToggleRunFPS = 0;
			}
		}
	}

	V4D2 vca = vc.component2V(var);
	V4D2 vcb = vc.component2V(vbr);
	V4D2 vcc = vc.component2V(vcr);

	V4D2 vcproj = V4D2(vc[0] - vcb[0], vc[1] - vcb[1], vc[2] - vcb[2]);
	double vcprojL = vcproj.length();
	const double cosMLV = cos(max_look_vertical);

	if (vcprojL == 0)
	{
		vcproj = vcc;
		vcproj *= cosMLV;
		vcprojL = vcproj.length();
	}
	else if (vcprojL < cosMLV)
	{
		vcproj *= cosMLV / vcprojL;
		vcprojL = cosMLV;
	}

	vca = vcproj.component2V(var);
	vcc = vcproj.component2V(vcr);
	vcb *= 1 / vcprojL;

	if (VxD::sign2V(vcc, vcr) == -1 && !RunState)
	{
		vcproj = var * vca.sign2V(var);
	}

	const V4D2 vcprojcy = vcproj.normalize();
	vca = vcprojcy.component2V(var);
	vcc = vcprojcy.component2V(vcr);

	if (!(RunState || RunTimeout > 0.0f))
	{
		const double vcaL = vca.length();
		const double sinMLH = TrackHeadLocal ? sin(RAD(30.0)) : sin(max_look_horizontal);

		if (vcaL > sinMLH)
		{
			const double k = sinMLH * vca.sign2V(var);
			vca = var * k;
			const double cosMLH = TrackHeadLocal ? cos(RAD(30.0)) : cos(max_look_horizontal);
			vcc = vcr * cosMLH;
		}
	}
	else if (VxD::sign2V(vcc, vcr) == -1)
	{
		const double vcaL = vca.length();
		const double sinMLH = sin(RAD(135.0));

		if (vcaL < sinMLH)
		{
			const double k = sinMLH * vca.sign2V(var);
			vca = var * k;
			const double cosMLH = cos(RAD(135.0));
			vcc = vcr * cosMLH;
		}
	}

	vc = vca + vcb + vcc;
	vc.normalize();
	va = vbr.cross(vc).normalize();
	vb = vc.cross(va).normalize();

	if (!TrackHeadLocal)
	{
		PRot = -static_cast<float>(atan2(-va[2], -va[0]));
		cosPRot = cos(PRot);
		sinPRot = sin(PRot);
	}

	V4D2 va2 = { -cosPRot, 0, sinPRot };
	V4D2 vb2 = { 0, 1, 0 };
	V4D2 vc2 = { -sinPRot, 0, -cosPRot };

	const double max_vAng = max_look_vertical * 0.8;
	const double vAng = std::clamp(-atan2(vc[1], sqrt(vc[0] * vc[0] + vc[2] * vc[2])), -max_vAng, max_vAng);
	V4D2 Q_V = V4D2(va2, vAng);

	vc2.qTransform(Q_V);
	vb2 = vc2.cross(va2).normalize();

	const double OffsetArmor = GetOffsetArmor() + cam_adjust_height;
	const double OffsetWeapon = GetOffsetWeapon() - sqrt(OffsetArmor) * 0.1 + 0.01516 + cam_adjust_horizontal;

	const V4D2 vdo = V4D2(reinterpret_cast<float*>(TraversePtr(HeadBonePosPtr)));
	const V4D2 vd = { vdo[0] + va[0] * OffsetWeapon + vb[0] * cam_adjust_roll + vc[0] * cam_adjust_depth, vdo[1] + OffsetArmor, vdo[2] + va[2] * OffsetWeapon + vb[2] * cam_adjust_roll + vc[2] * cam_adjust_depth, 1 };
	V4D2 vd2 = reinterpret_cast<float*>(TraversePtr(PlayerPosPtr));

	VxD2::write_vmtx(va, vb, vc, vd, dcam_mtx);
	VxD2::write_vmtx(va, vb, vc, vd2, follow_mtx);
	VxD2::write_vmtx(va, vb, vc, vd, aim_mtx);

	VxD2::write(vd2, reinterpret_cast<float*>(TraversePtr(AimCamBasePtr)));

	vd2 += vc2 * -0.04;

	if (!(IsLockHeld || TrackHeadLocal))
	{
		VxD2::write_vmtx(va2, vb2, vc2, vd2, base_mtx); 
		if (MovementInput.length() < 0.37 && MaxRotationHdegs == INFINITY && !RunState && !ObjActState && SyncRot)
		{
			*pPRot = PRot;
		}
	}
	else
	{
		VxD2::write(vd2, base_mtx->mtx[3]);
	}

	RunStateB = RunState;
	Q_Prev = Q;
}
