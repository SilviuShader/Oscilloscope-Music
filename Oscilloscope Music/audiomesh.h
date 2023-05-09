#pragma once
#include <raylib.h>

#include "audiodata.h"

Vector2 AudioToNDC(float, float);
Mesh GenFrameMesh(AudioData, float, int, int);