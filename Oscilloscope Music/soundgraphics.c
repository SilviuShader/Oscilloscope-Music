#include "soundgraphics.h"

#include <raylib.h>
#include <raymath.h>

#include "audiographics.h"

void GenerateAudioFrame(FrameGeneratorState* state, AudioData* audioData, float deltaTime)
{
	AudioPath audioPath = CreateAudioPath(1);

	/*
	float frequency = 261.63f * 10.0f;
	float amplitude = 0.01f;

	AudioPoint audioPoint;
	audioPoint.frequency = frequency;
	audioPoint.amplitude = amplitude;
	audioPoint.position.x = -0.5f;
	audioPoint.position.y = -0.5f;
	AppendAudioPoint(&audioPath, audioPoint);

	audioPoint.position.x = 0.5f;
	AppendAudioPoint(&audioPath, audioPoint);

	audioPoint.position.y = 0.5f;
	AppendAudioPoint(&audioPath, audioPoint);

	audioPoint.position.x = -0.5f;
	AppendAudioPoint(&audioPath, audioPoint);

	audioPoint.position.y = -0.5f;
	AppendAudioPoint(&audioPath, audioPoint);
	*/

	Matrix worldMatrix = MatrixRotateY(state->accumulatedTime * 0.1f);
	worldMatrix = MatrixMultiply(worldMatrix, MatrixRotateZ(state->accumulatedTime * 0.05f));
	worldMatrix = MatrixMultiply(worldMatrix, MatrixRotateX(state->accumulatedTime * 0.20f));

	float bpm = 120.0f;

	float scale = (sinf(2.0f * PI * state->accumulatedTime * (bpm / 60.0f)) * 0.5f + 0.5f) * 0.4f + 0.1f;
	worldMatrix = MatrixMultiply(worldMatrix, MatrixScale(scale, scale, scale));
	worldMatrix = MatrixMultiply(worldMatrix, MatrixTranslate(-1.0f, -0.5f, 0.0f));

	AppendCube(&audioPath, worldMatrix);

	//worldMatrix = MatrixMultiply(worldMatrix, MatrixTranslate(1.0f, 0.0f, 0.0f));
	//AppendCube(&audioPath, worldMatrix);

	AppendAudioPath(audioData, audioPath, deltaTime);
	state->accumulatedTime += deltaTime;

	// TODO: Ewwww
	FreeAudioPath(&audioPath);
}

void AppendAudioPath(AudioData* audioData, AudioPath audioPath, float deltaTime)
{
	float totalDistance = 0.0f;
	for (int i = 1; i < audioPath.pointsCount; i++)
		totalDistance += Vector2Distance(audioPath.audioPath[i - 1].position, audioPath.audioPath[i].position);

	float numSamples = audioData->sampleRate * deltaTime;
	float distanceBetweenSamples = totalDistance / numSamples;

	float accumulatedDistance = 0.0f;
	float accumulatedLineDistance = 0.0f;

	float accumulatedTime = 0.0f;

	int currentPoint = 0;

	while (accumulatedDistance < totalDistance)
	{
		int nextPoint = currentPoint + 1;
		if (nextPoint >= audioPath.pointsCount)
			break;

		float segmentLength = Vector2Distance(audioPath.audioPath[currentPoint].position, audioPath.audioPath[nextPoint].position);
		float t = accumulatedLineDistance / segmentLength;

		// TODO: Hard-coded bias
		if (segmentLength < 0.001f || t >= 1.0f)
		{
			currentPoint++;
			accumulatedLineDistance = 0.0f;// TODO: Adjust remaining t
			continue;
		}

		Vector2 targetPosition = Vector2Lerp(audioPath.audioPath[currentPoint].position, audioPath.audioPath[nextPoint].position, t);
		float targetFrequency = Lerp(audioPath.audioPath[currentPoint].frequency, audioPath.audioPath[nextPoint].frequency, t);
		float targetAmplitude = Lerp(audioPath.audioPath[currentPoint].amplitude, audioPath.audioPath[nextPoint].amplitude, t);

		Vector2 audioSample = targetPosition;

		audioSample.x += targetAmplitude * cosf(2.0f * PI * accumulatedTime * targetFrequency);
		audioSample.y += targetAmplitude * sinf(2.0f * PI * accumulatedTime * targetFrequency);

		SetAudioSample(audioData, audioData->samplesCount, audioSample, LEFT_CHANNEL | RIGHT_CHANNEL);

		accumulatedLineDistance += distanceBetweenSamples;
		accumulatedDistance += distanceBetweenSamples;

		accumulatedTime += 1.0f / (float)audioData->sampleRate;
	}
}
