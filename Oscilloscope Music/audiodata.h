#pragma once
#include <raylib.h>
#include <stddef.h>

// TODO: Class that contains the float arrays + reserved bytes + used bytes and methods that dynamically extend the arrays when appending sound

// TODO: Drawing functions for a specified amount of time + create song loop.

#define DEFAULT_AUDIO_SAMPLES_COUNT 1024

#define LEFT_CHANNEL                0b1
#define RIGHT_CHANNEL               0b10

typedef struct AudioData
{
	float* leftSamples;
	float* rightSamples;
	size_t samplesCount;
	size_t reserved;
	int    sampleRate;
} AudioData;

AudioData CreateAudioData(size_t, int sample_rate);
void FreeAudioData(AudioData*);
void AutoExtendAudioData(AudioData*, size_t);

void SetAudioSample(AudioData*, int, Vector2, int);
Vector2 GetAudioSample(const AudioData*, int, int);