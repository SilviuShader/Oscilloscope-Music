#pragma once

#include "audiopath.h"

void AppendCube(AudioPath*, Matrix, Matrix, Matrix);
void AppendSphere(float, float, AudioPath*, Matrix, Matrix, Matrix);

void AppendPath3D(Vector3*, size_t, AudioPath*, Matrix, Matrix, Matrix);
Vector4 Vector4Transform(Vector4 v, Matrix mat);