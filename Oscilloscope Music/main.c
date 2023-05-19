#include <corecrt_math.h>
#include <stdint.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audiodata.h"
#include "audiomesh.h"
#include "soundgraphics.h"
#include "wavhelper.h"

AudioData GenerateAudioData(uint32_t sampleRate, int targetFPS, float animationTime)
{
    AudioData audioData = CreateAudioData(DEFAULT_AUDIO_SAMPLES_COUNT, sampleRate);

    const float deltaTime = 1.0f / (float)targetFPS;
          float accumulatedTime = 0.0f;

    FrameGeneratorState state;
    state.accumulatedTime = 0.0f;

    while (accumulatedTime < animationTime)
    {
        GenerateAudioFrame(&state, &audioData, deltaTime);
        accumulatedTime += deltaTime;
    }

    return audioData;
}


AudioData InterpretConfigFile(uint32_t sampleRate, int targetFPS, float animationTime)
{
    int generate = 0;
    char filename[128] = { NULL };

	FILE* configFile = fopen("config.txt", "r");
    if (!configFile)
        generate = 1;
    else
    {
        if (fscanf(configFile, "%s", filename))
            if (!IsFileExtension(filename, ".wav"))
                generate = 1;

        fclose(configFile);
    }

    if (generate == 0)
    {
        FILE* musicFile = fopen(filename, "r");
        if (!musicFile)
            generate = 1;
        else
			fclose(musicFile);
    }

    AudioData audioData;

    if (generate)
        audioData = GenerateAudioData(sampleRate, targetFPS, animationTime);
    else
        audioData = LoadAudioDataFromFile(filename);

    return audioData;
}


float MoveTowards(float from, float to, float maxDelta)
{
    float result = from;

    if (result < to)
        result = min(result + maxDelta, to);
    else
        result = max(result - maxDelta, to);

    return result;
}

Vector2 UpdateVU(Vector2 vu, AudioData* audioData, int mask, int firstSampleIndex, int lastSampleIndex, float deltaTime)
{
    const float dt = deltaTime / (lastSampleIndex - firstSampleIndex);
    const float riseTime = 0.3f;
    const float acceleration = 100.0f;

    const float speed = 1.0f / riseTime;

	for (int i = firstSampleIndex; i < lastSampleIndex; i++)
    {
        float currentLevel = vu.x;
        float currentSpeed = vu.y;

        float targetSpeed = 0.0f;

		const Vector2 sample = GetAudioSample(audioData, i, mask);

		float s = sample.x;
        if (fabsf(sample.y) > fabsf(s))
            s = sample.y;
        
        s *= 0.5f;
        s += 0.5f;

        if (s > currentLevel)
            targetSpeed = speed;
        else
            targetSpeed = -speed;

        const float maxSpeedChange = acceleration * dt;

        currentSpeed = MoveTowards(currentSpeed, targetSpeed, maxSpeedChange);
        currentLevel += currentSpeed * dt;

        vu.x = currentLevel;
        vu.y = currentSpeed;
    }

    return vu;
}

void DrawVU(Vector2 position, float vuLevel)
{
    Vector2 direction = { 0, -1 };

    float minAngle = -75.0f;
    float maxAngle = 75.0f;

    Vector2 length = { 50.0f, 50.0f };

    float angle = DEG2RAD * Lerp(minAngle, maxAngle, vuLevel);

    direction = Vector2Rotate(direction, angle);

    DrawLineEx(position, Vector2Add(position, Vector2Multiply(direction, length)), 1.0f, RAYWHITE);
    DrawText("VU", position.x - 10.0f, position.y + 10.0f, 20, RAYWHITE);
}

int main(void)
{
	const uint32_t screenWidth    = 800;
	const uint32_t screenHeight   = 800;
                                  
    const int      targetFPS      = 60;
                                  
          uint32_t sampleRate     = 48000;
                                  
          float    animationTime  = 30.0f;

	const float    uIntensity     = 0.1f;
	const float    uSize          = 0.02f;
    const float    uIntensityBase = max(0.0f, uIntensity - 0.4f) * 0.7f - 1000.0f * uSize / 500.0f;

    const float    aspectRatio    = (float)screenWidth / (float)screenHeight;

	InitWindow(screenWidth, screenHeight, "Oscilloscope Music");
	SetTargetFPS(targetFPS);

    AudioData audioData = InterpretConfigFile(sampleRate, targetFPS, animationTime);

    sampleRate = audioData.sampleRate;

    animationTime = (float)audioData.samplesCount / (float)sampleRate;
    
    uint16_t* wavSamples = CreateStereoWavSamples(audioData.leftSamples, audioData.rightSamples, audioData.samplesCount);
    uint8_t* wavData;
    uint32_t wavDataLength = CreateWav16PCMStereo(wavSamples, sampleRate, audioData.samplesCount, &wavData);

    FILE* audioFile = fopen("test.wav", "wb");
    fwrite(wavData, sizeof(uint8_t), wavDataLength, audioFile);
    fclose(audioFile);

    InitAudioDevice();
    Music music = LoadMusicStreamFromMemory(".wav", wavData, wavDataLength);
    PlayMusicStream(music);

    float previousMusicTimePlayed = 0.0f;

    Camera camera = { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, 2.0f, CAMERA_ORTHOGRAPHIC };

    Shader shader = LoadShader("./data/vs.glsl", "./data/fs.glsl");
    SetShaderValue(shader, GetShaderLocation(shader, "aspectRatio"), &aspectRatio, SHADER_UNIFORM_FLOAT);

	SetShaderValue(shader, GetShaderLocation(shader, "uIntensity"), &uIntensity, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, GetShaderLocation(shader, "uSize"), &uSize, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, GetShaderLocation(shader, "uIntensityBase"), &uIntensityBase, SHADER_UNIFORM_FLOAT);

    Vector2 leftVU = Vector2Zero();
    Vector2 rightVU = Vector2Zero();

	while (!WindowShouldClose())
	{
        float currentMusicTimePlayed = GetMusicTimePlayed(music);

        if (currentMusicTimePlayed < previousMusicTimePlayed)
            previousMusicTimePlayed = 0.0f;

        int firstSampleIndex = (int)(previousMusicTimePlayed / animationTime * (float)audioData.samplesCount);
        int lastSampleIndex = (int)(currentMusicTimePlayed / animationTime * (float)audioData.samplesCount);

        Mesh mesh = GenFrameMesh(audioData, uSize, firstSampleIndex, lastSampleIndex);
        Model model = LoadModelFromMesh(mesh);

        float deltaTime = currentMusicTimePlayed - previousMusicTimePlayed;

        leftVU = UpdateVU(leftVU, &audioData, LEFT_CHANNEL, firstSampleIndex, lastSampleIndex, deltaTime);
        rightVU = UpdateVU(rightVU, &audioData, RIGHT_CHANNEL, firstSampleIndex, lastSampleIndex, deltaTime);

        model.materials[0].shader = shader;

        UpdateMusicStream(music);

		BeginDrawing();

        ClearBackground(BLACK);

        BeginMode3D(camera);
        BeginBlendMode(BLEND_ALPHA);
        DrawModelEx(model, Vector3Zero(), (Vector3) { 0.0f, 1.0f, 0.0f }, 0.0f, (Vector3) { 1.0f, (float)screenHeight / (float)screenWidth, 1.0f }, GREEN);
        EndBlendMode();
		EndMode3D();

        Vector2 leftVUPosition = { 100, screenHeight - 100 };
        Vector2 rightVUPosition = { screenWidth - 100, screenHeight - 100 };


        DrawVU(leftVUPosition, leftVU.x);
        DrawVU(rightVUPosition, rightVU.x);

		EndDrawing();

        UnloadModel(model);
        
        previousMusicTimePlayed = currentMusicTimePlayed;
	}

    UnloadShader(shader);

    FreeWav(wavData);
    FreeWavSamples(wavSamples);

    FreeAudioData(&audioData);

	return 0;
}