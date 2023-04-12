#include "audiographics.h"

#include <raymath.h>

void AppendCube(AudioPath* audioPath, Matrix worldMatrix)
{
	// TODO: Back to start point;
	const Vector3 path3d[] = 
	{
		// front bottom segment
		{-1.f, -1.f, -1.f},
		{ 1.f, -1.f, -1.f},

		// front right segment
		{ 1.f, -1.f, -1.f},
		{ 1.f,  1.f, -1.f},

		// front top segment
		{ 1.f,  1.f, -1.f},
		{ -1.f,  1.f, -1.f},

		// front left segment
		{ -1.f,  1.f, -1.f},
		{ -1.f,  -1.f, -1.f},

		// left bottom segment
		{ -1.f,  -1.f, -1.f},
		{ -1.f,  -1.f,  1.f},

		// left back segment
		{ -1.f,  -1.f,  1.f},
		{ -1.f,   1.f,  1.f},

		// left top segment
		{ -1.f,   1.f,  1.f},
		{ -1.f,   1.f,  -1.f},

		// front top segment
		{ -1.f,   1.f,  -1.f},
		{ 1.f,   1.f,  -1.f},

		// right top segment
		{ 1.f,   1.f,  -1.f},
		{ 1.f,   1.f,   1.f},

		// right back segment
		{ 1.f,   1.f,   1.f},
		{ 1.f,   -1.f,   1.f},

		// back bottom segment
		{ 1.f,   -1.f,   1.f},
		{ -1.f,   -1.f,   1.f},

		// back left segment
		{ - 1.f,   -1.f,   1.f},
        {  -1.f,   1.f,   1.f},

		// back top segment
		{ -1.f,   1.f,   1.f},
        {  1.f,   1.f,   1.f},

		// right back segment
		{  1.f,   1.f,   1.f},
		{  1.f,   -1.f,   1.f},

		// right bottom segment
		{  1.f,   -1.f,   1.f},
		{  1.f,   -1.f,   -1.f},

		// front bottom segment
		{  1.f,   -1.f,   -1.f},
		{  -1.f,   -1.f,   -1.f}
	};

	const int pointsCount = sizeof(path3d) / sizeof(Vector3);

	for (int i = 0; i < pointsCount; i++)
	{
		Vector3 point3D = path3d[i];
		point3D.x *= 0.5f;
		point3D.y *= 0.5f;
		point3D.z *= 0.5f;
		point3D = Vector3Transform(point3D, worldMatrix);
		AudioPoint audioPoint;

		audioPoint.position.x = point3D.x;
		audioPoint.position.y = point3D.y;

		audioPoint.frequency = 261.63f * 10.0f;
		audioPoint.amplitude = 0.001f;

		AppendAudioPoint(audioPath, audioPoint);
	}
}
