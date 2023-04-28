#include "audiographics.h"

#include <raymath.h>

void AppendCube(AudioPath* audioPath, Matrix worldMatrix, Matrix viewMatrix, Matrix projectionMatrix)
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
		Vector4 point3D;
		point3D.x = path3d[i].x * 0.5f;
		point3D.y = path3d[i].y * 0.5f;
		point3D.z = path3d[i].z * 0.5f;
		point3D.w = 1.0f;

		point3D = Vector4Transform(point3D, worldMatrix);
		point3D = Vector4Transform(point3D, viewMatrix);
		point3D = Vector4Transform(point3D, projectionMatrix);

		point3D.x /= point3D.w;
		point3D.y /= point3D.w;
		point3D.z /= point3D.w;
		point3D.w /= point3D.w;

		AudioPoint audioPoint;

		audioPoint.position.x = point3D.x;
		audioPoint.position.y = point3D.y;

		audioPoint.frequency = 261.63f * 10.0f;
		audioPoint.amplitude = 0.001f;

		AppendAudioPoint(audioPath, audioPoint);
	}
}

Vector4 Vector4Transform(Vector4 v, Matrix mat)
{
	Vector4 result = { 0 };

	float x = v.x;
	float y = v.y;
	float z = v.z;
	float w = v.w;

	result.x = mat.m0 * x + mat.m4 * y + mat.m8 * z + mat.m12;
	result.y = mat.m1 * x + mat.m5 * y + mat.m9 * z + mat.m13;
	result.z = mat.m2 * x + mat.m6 * y + mat.m10 * z + mat.m14;
	result.w = mat.m3 * x + mat.m7 * y + mat.m11 * z + mat.m15;

	return result;
}
