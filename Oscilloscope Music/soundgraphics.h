#pragma once
#include "audiodata.h"
#include "audiopath.h"

typedef struct FrameGeneratorState
{
	float accumulatedTime;
	float aspectRatio;
} FrameGeneratorState;

void GenerateAudioFrame(FrameGeneratorState*, AudioData*, float);

void AppendAudioPath(AudioData*, AudioPath, float);
