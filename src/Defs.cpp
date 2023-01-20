#include "Defs.h"

extern int fp_fov_base = 90;

extern bool track_roll = true;
extern bool track_hit = true;
extern bool test_out = false;
extern bool use_virtualalloc2 = true;

extern const double max_look_v = RAD(70.0);

extern const double max_look_htrack = RAD(30.0);
extern const double max_look_hlocked = RAD(80.0);
extern const double max_look_hfree = RAD(135.0);

extern volatile double cam_adjust_height = 0.03;
extern volatile double cam_adjust_horizontal = 0.01;
extern volatile double cam_adjust_roll = 0.17;
extern volatile double cam_adjust_depth = 0.01501;

extern std::map<uint32_t, bool> NoSyncAnimationIDs = { { 19022, true }, { 19120, true }, { 28999, true }, { 36210, true }, { 36215, true },
													   { 39000, true }, { 39010, true }, { 39020, true }, { 39040, true }, { 50091, true } };

extern const float UpperArmParamData[20] = { 5.0f, 30.0f, 10.0f, 7.0f, 10.0f, 15.0f, 0.0f, 30.0f, 0.0f, 0.0f, 0.0f, 15.0f, 0.0f, 25.0f, 0.0f, 40.0f, 0.0f, 25.0f, 0.0f, 40.0f };

volatile extern bool IsFPS = false;
volatile extern bool* pIsFPS = &IsFPS;

volatile extern bool ToggleRunFPS = false;
volatile extern bool* pToggleRunFPS = &ToggleRunFPS;

volatile extern bool IsBinocsFPS = false;
volatile extern bool* pIsBinocsFPS = &IsBinocsFPS;

volatile extern bool CamInit = false;

volatile extern alignas(16) std::atomic<float> dT = 0.0f;

volatile extern alignas(16) std::atomic<float> MaxRotation = INFINITY;

volatile extern bool TrackHead = false;

volatile extern bool SyncRot = false;

extern MODULEINFO mInfo = {};