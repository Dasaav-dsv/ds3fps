#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Psapi.h>

#include <atomic>
#include <map>

#pragma warning(push)
#pragma warning(disable : 6294 6387)
#include "VxD.h"
#include "Pointer.h"
#pragma warning(pop)

#define M_PIf 3.14159265358979323846f
#define M_PI_180f 0.0174532923847436904f
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_180
#define M_PI_180 0.01745329251994329547
#endif

#define RADf(deg) (deg * M_PI_180f)
#define RAD(deg) (deg * M_PI_180)

extern int fp_fov_base;

extern bool track_roll;
extern bool track_hit;
extern bool test_out;
extern bool use_virtualalloc2;

extern const double max_look_vertical;
extern const double max_look_horizontal;

extern volatile double cam_adjust_height;
extern volatile double cam_adjust_horizontal;
extern volatile double cam_adjust_roll;
extern volatile double cam_adjust_depth;

extern std::map<uint32_t, bool> NoSyncAnimationIDs;

extern const float UpperArmParamData[20];

volatile extern bool IsFPS;
volatile extern bool* pIsFPS;

volatile extern bool ToggleRunFPS;
volatile extern bool* pToggleRunFPS;

volatile extern bool CamInit;

volatile extern alignas(16) std::atomic<float> dT;

volatile extern alignas(16) std::atomic<float> MaxRotation;

volatile extern bool TrackHead;

volatile extern bool SyncRot;

extern MODULEINFO mInfo;