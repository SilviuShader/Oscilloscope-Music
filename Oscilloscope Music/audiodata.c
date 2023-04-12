#include "audiodata.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AudioData CreateAudioData(const size_t size, const int sampleRate)
{
	AudioData audioData;

	audioData.leftSamples = malloc(sizeof(float) * size);
	if (!audioData.leftSamples)
		printf("Cannot allocate audio samples.");
	memset(audioData.leftSamples, 0, sizeof(float) * size);

	audioData.rightSamples = malloc(sizeof(float) * size);
	if (!audioData.rightSamples)
		printf("Cannot allocate audio samples.");
	memset(audioData.rightSamples, 0, sizeof(float) * size);

	audioData.reserved = size;
	audioData.samplesCount = 0;
	audioData.sampleRate = sampleRate;

	return audioData;
}

void FreeAudioData(AudioData* audioData)
{
	free(audioData->leftSamples);
	free(audioData->rightSamples);

	audioData->leftSamples = NULL;
	audioData->rightSamples = NULL;

	audioData->reserved = 0;
	audioData->samplesCount = 0;
	audioData->sampleRate = 0;
}

void AutoExtendAudioData(AudioData* audioData, size_t desiredSize)
{
	if (audioData->reserved >= desiredSize)
		return;

	AudioData newData = CreateAudioData(audioData->reserved << 1, audioData->sampleRate);
	memcpy(newData.leftSamples, audioData->leftSamples, sizeof(float) * audioData->samplesCount);
	memcpy(newData.rightSamples, audioData->rightSamples, sizeof(float) * audioData->samplesCount);
	newData.samplesCount = audioData->samplesCount;

	FreeAudioData(audioData);
	*audioData = newData;
}

void SetAudioSample(AudioData* audioData, const int index, const Vector2 values, const int mask)
{
	if (index >= audioData->reserved)
		AutoExtendAudioData(audioData, index + 1);

	audioData->samplesCount = max(audioData->samplesCount, index + 1);

	audioData->leftSamples[index]  = values.x * (float)((mask & LEFT_CHANNEL)  != 0) + audioData->leftSamples[index]  * (1.0f - (float)((mask & LEFT_CHANNEL) != 0));
	audioData->rightSamples[index] = values.y * (float)((mask & RIGHT_CHANNEL) != 0) + audioData->rightSamples[index] * (1.0f - (float)((mask & RIGHT_CHANNEL) != 0));
}

Vector2 GetAudioSample(const AudioData* audioData, const int index, const int mask)
{
	if (index >= audioData->samplesCount)
		printf("Trying to access non-existing samples.");

	Vector2 output;

	output.x = audioData->leftSamples[index]  * (float)((mask & LEFT_CHANNEL)  != 0);
	output.y = audioData->rightSamples[index] * (float)((mask & RIGHT_CHANNEL) != 0);

	return output;
}
