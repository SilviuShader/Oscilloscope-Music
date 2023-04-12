#pragma once

#include <raylib.h>

typedef struct AudioPoint
{
	Vector2 position;
	float   amplitude;
	float   frequency;
} AudioPoint;

typedef struct AudioPath
{
	AudioPoint* audioPath;
	size_t      pointsCount;
	size_t      reserved;
} AudioPath;


AudioPath CreateAudioPath(size_t);
void FreeAudioPath(AudioPath*);
void AutoExtendAudioPath(AudioPath*, size_t);
void AppendAudioPoint(AudioPath*, AudioPoint);
void ClearAudioPath(AudioPath*);