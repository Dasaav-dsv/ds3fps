#pragma once

#include "TAE.h"

static float SyncTime = 0.0f;

static const PDATA TAEHeaderPtr[] = {&TAE_PtrBase, 0x8, 0x18, 0x0};
static const PDATA EventCountPtr[] = {&TAE_PtrBase, 0x20};
static const PDATA EventTAEBasePtr[] = {&TAE_PtrBase, 0x0, 0x0};

inline bool CheckTAETime(const float AnimTime, const float EventStartTime, const float EventEndTime)
{
	if (EventStartTime <= AnimTime && AnimTime <= EventEndTime)
	{
		return true;
	}
	else
	{
		return false;
	}
}

extern void GetTAEData()
{
	bool TrackHeadLocal = true;
	bool TrackHeadLocalOverride = false;
	float MaxRotationLocal = INFINITY;

	if (*CSLang == 8)
	{
		track_roll = *TraversePtr(TrackDodgesPtr) == 1;
	}

	uint32_t CurrentAnimationID = *reinterpret_cast<uint32_t*>(TraversePtr(AnimIDPtr));
	CurrentAnimationID = CurrentAnimationID - CurrentAnimationID / 100000 * 100000;
	const uint32_t CurrentAnimationIndex = CurrentAnimationID - CurrentAnimationID / 10 * 10;
	const float AnimTime = *reinterpret_cast<float*>(TraversePtr(AnimTimePtr));

	const void* TAEHeader = reinterpret_cast<void*>(TraversePtr(TAEHeaderPtr));
	const uint16_t* pEventCount = reinterpret_cast<uint16_t*>(TraversePtr(EventCountPtr));

	if (TAEHeader > pEventCount)
	{
		return;
	}
	const uint16_t EventCount = *pEventCount;

	for (int16_t i = 0; i < EventCount; i++)
	{
		const PDATA EventTAECurrentPtr[] = {&TAE_PtrBase, 0x0, 0x18 * i + 0x10, 0x0};

		const uint32_t* pCurrentEventID = reinterpret_cast<uint32_t*>(TraversePtr(EventTAECurrentPtr));
		const uint32_t* pTAETableArg1 = reinterpret_cast<uint32_t*>(TraversePtr(EventTAECurrentPtr, 0x10));

		if (TAEHeader > pCurrentEventID || TAEHeader > pTAETableArg1) // bounds check for bad pointers 
		{
			return;
		}

		const uint32_t CurrentEventID = *pCurrentEventID;
		const uint32_t TAETableArg1 = *pTAETableArg1;

		const float CurrentEventStartTime = *reinterpret_cast<float*>(TraversePtr(EventTAEBasePtr, 0x18 * i));
		const float CurrentEventEndTime = *reinterpret_cast<float*>(TraversePtr(EventTAEBasePtr, 0x18 * i + 0x8));

		switch (CurrentEventID)
		{
		case 0:
		case 66:
		case 67:
		case 96:
		case 600:
			switch (TAETableArg1 >= 100000 && TAETableArg1 <= 100150 || TAETableArg1 == 100330 || TAETableArg1 == 440602 ? 85 : TAETableArg1)
			{
			case 3:
			case 6:
			case 15:
			case 28:
			case 64:
			case 85:
				TrackHeadLocal = false;
				break;
			case 7:
				if (CheckTAETime(AnimTime, CurrentEventStartTime, CurrentEventEndTime) && MaxRotationLocal == INFINITY)
				{
					MaxRotationLocal = 0.0f;
				}
				break;
			case 66:
				TrackHeadLocal = track_roll;
				break;
			case 71:
				TrackHeadLocal = track_hit;
			}
			break;
		case 1:
		case 2:
		case 64:
		case 194:
		case 232:
		case 331:
		case 790:
			TrackHeadLocal = false;
			break;
		case 304:
			TrackHeadLocalOverride = true;
			break;
		case 307:
			if (*reinterpret_cast<uint32_t*>(TraversePtr(EventTAECurrentPtr, 0x18)) == 504)
			{
				if (track_roll)
				{
					SyncTime = 0.25f;
				}
				else
				{
					TrackHeadLocal = false;
				}
			}
			else
			{
				TrackHeadLocal = true;
			}
			break;
		case 224:
			if (CheckTAETime(AnimTime, CurrentEventStartTime, CurrentEventEndTime) && MaxRotationLocal == INFINITY)
			{
				MaxRotationLocal = *reinterpret_cast<float*>(TraversePtr(EventTAECurrentPtr, 0x10));
			}
			break;
		case 703:
			if (CheckTAETime(AnimTime, CurrentEventStartTime, CurrentEventEndTime) && MaxRotationLocal == INFINITY)
			{
				MaxRotationLocal = 0.0f;
			}
		}
	}

	if (!(CurrentAnimationID >= 5300 && CurrentAnimationID < 5310) && ((CurrentAnimationID >= 5000 && CurrentAnimationID < 5500) || CurrentAnimationID >= 10000 && CurrentAnimationID < 15000)) // small hurt animations check
	{
		TrackHeadLocal = false;
		goto AnimIDCheckEnd;
	}

	if (CurrentAnimationID >= 28000 && CurrentAnimationID < 29000) // ladder animations
	{
		MaxRotationLocal = 0.0f;
		goto AnimIDCheckEnd;
	}

	if (CurrentAnimationID == 35500 || CurrentAnimationID == 36500)
	{
		SyncRot = false;
		goto AnimIDCheckEnd;
	}
	else
	{
		SyncRot = true;
	}

	if (CurrentAnimationID == 50091)
	{
		IsBinocsFPS = true;
		SyncRot = false;
		goto AnimIDCheckEnd;
	}
	else
	{
		IsBinocsFPS = false;
		SyncRot = true;
	}

	if (CurrentAnimationID >= 68000 && CurrentAnimationID < 80000) // interact and throw animations
	{
		TrackHeadLocalOverride = true;
		goto AnimIDCheckEnd;
	}

AnimIDCheckEnd:
	TrackHeadLocal = NoSyncAnimationIDs[CurrentAnimationID] ? false : TrackHeadLocal;

	MaxRotation.exchange(MaxRotationLocal);

	if (SyncTime > 0.0)
	{
		SyncTime -= dT.load();
		TrackHead = true;
	}
	else
	{
		SyncTime = 0.0;
		TrackHead = TrackHeadLocal || TrackHeadLocalOverride;
	}
}
