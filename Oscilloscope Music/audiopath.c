#include "audiopath.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AudioPath CreateAudioPath(const size_t size)
{
	AudioPath audioPath;

	audioPath.audioPath = malloc(sizeof(AudioPoint) * size);
	if (!audioPath.audioPath)
		printf("Cannot allocate audio path.");

	memset(audioPath.audioPath, 0, sizeof(AudioPoint) * size);
	audioPath.reserved = size;
	audioPath.pointsCount = 0;

	return audioPath;
}

void FreeAudioPath(AudioPath* audioPath)
{
	free(audioPath->audioPath);
	audioPath->audioPath = NULL;
	audioPath->pointsCount = 0;
	audioPath->reserved = 0;
}

void AutoExtendAudioPath(AudioPath* audioPath, size_t desiredSize)
{
	if (audioPath->reserved >= desiredSize)
		return;

	AudioPath newPath = CreateAudioPath(audioPath->reserved << 1);
	memcpy(newPath.audioPath, audioPath->audioPath, sizeof(AudioPoint) * audioPath->pointsCount);
	newPath.pointsCount = audioPath->pointsCount;

	FreeAudioPath(audioPath);
	*audioPath = newPath;
}

void AppendAudioPoint(AudioPath* audioPath, AudioPoint audioPoint)
{
	size_t desiredSize = audioPath->pointsCount + 1;
	AutoExtendAudioPath(audioPath, desiredSize);

	audioPath->audioPath[audioPath->pointsCount++] = audioPoint;
}

void ClearAudioPath(AudioPath* audioPath)
{
	audioPath->pointsCount = 0;
}