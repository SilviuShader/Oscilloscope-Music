#include <corecrt_math.h>
#include <stdint.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

#include "audiodata.h"
#include "audiomesh.h"
#include "soundgraphics.h"
#include "wavhelper.h"

void Normalize01Wave(AudioData* audioData)
{
    for (int i = 0; i < audioData->samplesCount; i++)
    {
        const int mask = LEFT_CHANNEL | RIGHT_CHANNEL;
        Vector2 currentData = GetAudioSample(audioData, i, mask);

        currentData.x = (currentData.x + 1.0f) * 0.5f;
        currentData.y = (currentData.y + 1.0f) * 0.5f;

        currentData.x = Clamp(currentData.x, 0.0f, 1.0f);
        currentData.y = Clamp(currentData.y, 0.0f, 1.0f);

        SetAudioSample(audioData, i, currentData, mask);
    }
}

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

    Normalize01Wave(&audioData);

    return audioData;
}

int main(void)
{
	const uint32_t screenWidth    = 800;
	const uint32_t screenHeight   = 450;
                                  
    const int      targetFPS      = 60;
                                  
    const uint32_t sampleRate     = 48000;
                                  
          float    animationTime  = 30.0f;

	const float    uIntensity     = 0.1f;
	const float    uSize          = 0.02f;
    const float    uIntensityBase = max(0.0f, uIntensity - 0.4f) * 0.7f - 1000.0f * uSize / 500.0f;

    const float    aspectRatio    = (float)screenWidth / (float)screenHeight;

	InitWindow(screenWidth, screenHeight, "Oscilloscope Music");
	SetTargetFPS(targetFPS);

    AudioData audioData = GenerateAudioData(sampleRate, targetFPS, animationTime);

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

	while (!WindowShouldClose())
	{
        float currentMusicTimePlayed = GetMusicTimePlayed(music);

        if (currentMusicTimePlayed < previousMusicTimePlayed)
            previousMusicTimePlayed = 0.0f;

        int firstSampleIndex = (int)(previousMusicTimePlayed / animationTime * (float)audioData.samplesCount);
        int lastSampleIndex = (int)(currentMusicTimePlayed / animationTime * (float)audioData.samplesCount);

        Mesh mesh = GenFrameMesh(audioData, uSize, firstSampleIndex, lastSampleIndex);

        Model model = LoadModelFromMesh(mesh);
        model.materials[0].shader = shader;

        UpdateMusicStream(music);

		BeginDrawing();

        ClearBackground(BLACK);
        
        BeginMode3D(camera);
        DrawModelEx(model, Vector3Zero(), (Vector3) { 0.0f, 1.0f, 0.0f }, 0.0f, (Vector3) { 1.0f, (float)screenHeight / (float)screenWidth, 1.0f }, GREEN);
        EndMode3D();

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