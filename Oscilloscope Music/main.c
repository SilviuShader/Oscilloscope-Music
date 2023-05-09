#include <corecrt_math.h>
#include <stdint.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

#include "audiodata.h"
#include "soundgraphics.h"
#include "wavhelper.h"

void AddSineWave(AudioData* audioData, const int startIndex, const int samplesCount, const int channelMask, const float frequency, const float phase, const float amplitude)
{
    float t = 0.0f;
    for (int i = startIndex; i < startIndex + samplesCount; i++)
    {
        const float value = amplitude * sinf(PI * 2.0f * frequency * t + phase);

        Vector2 value2;
        value2.x = value;
        value2.y = value;

        SetAudioSample(audioData, i, value2, channelMask);

        t = (float)i / (float)audioData->sampleRate;
    }
}

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

Vector2 AudioToNDC(const float leftSample, const float rightSample)
{
    Vector2 res;

    res.x = leftSample * 2.0f - 1.0f;
    res.y = rightSample * 2.0f - 1.0f;
    
    return res;
}

Mesh GenFrameMesh(const AudioData audioData, const float uSize, const int firstSampleIndex, const int lastSampleIndex)
{
    Mesh mesh = { 0 };
    mesh.triangleCount = (lastSampleIndex - firstSampleIndex) * 2;
    mesh.vertexCount = mesh.triangleCount * 3;
    mesh.vertices = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));    // 3 vertices, 3 coordinates each (x, y, z)
    mesh.texcoords = (float*)MemAlloc(mesh.vertexCount * 2 * sizeof(float));   // 3 vertices, 2 coordinates each (x, y)
    mesh.normals = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));     // 3 vertices, 3 coordinates each (x, y, z)

    for (int i = firstSampleIndex; i < lastSampleIndex - 1; i++)
    {
        float leftSample = audioData.leftSamples[i];
        float nextLeftSample = audioData.leftSamples[i + 1];

        float rightSample = audioData.rightSamples[i];
        float nextRightSample = audioData.rightSamples[i + 1];

        Vector2 prevPosition = AudioToNDC(leftSample, rightSample);
        Vector2 nextPosition = AudioToNDC(nextLeftSample, nextRightSample);

        Vector2 dir = Vector2Subtract(nextPosition, prevPosition);
        float z = Vector2Length(dir);
        if (z > EPSILON)
        {
            dir.x /= z;
            dir.y /= z;
        }
        else
        {
            dir.x = 1.0f;
            dir.y = 0.0f;
        }

        dir.x *= uSize;
        dir.y *= uSize;

        Vector2 norm;
        norm.x = -dir.y;
        norm.y = dir.x;

        const int iVertex = (i - firstSampleIndex) * 18;
        const int iNormal = (i - firstSampleIndex) * 18;
        const int iTexCoords = (i - firstSampleIndex) * 12;

        // ===================== first triangle =====================

        mesh.vertices[iVertex] = prevPosition.x - dir.x - norm.x;
        mesh.vertices[iVertex + 1] = prevPosition.y - dir.y - norm.y;
        mesh.vertices[iVertex + 2] = 0.0f;

        mesh.normals[iNormal] = -uSize;
        mesh.normals[iNormal + 1] = -uSize;
        mesh.normals[iNormal + 2] = z;

        mesh.texcoords[iTexCoords] = 0;
        mesh.texcoords[iTexCoords + 1] = 0;


        mesh.vertices[iVertex + 3] = prevPosition.x - dir.x + norm.x;
        mesh.vertices[iVertex + 4] = prevPosition.y - dir.y + norm.y;
        mesh.vertices[iVertex + 5] = 0.0f;

        mesh.normals[iNormal + 3] = -uSize;
        mesh.normals[iNormal + 4] = uSize;
        mesh.normals[iNormal + 5] = z;

        mesh.texcoords[iTexCoords + 2] = 1;
        mesh.texcoords[iTexCoords + 3] = 0;


        mesh.vertices[iVertex + 6] = nextPosition.x + dir.x - norm.x;
        mesh.vertices[iVertex + 7] = nextPosition.y + dir.y - norm.y;
        mesh.vertices[iVertex + 8] = 0.0f;

        mesh.normals[iNormal + 6] = z + uSize;
        mesh.normals[iNormal + 7] = -uSize;
        mesh.normals[iNormal + 8] = z;

        mesh.texcoords[iTexCoords + 4] = 0;
        mesh.texcoords[iTexCoords + 5] = 1;

        // ===================== second triangle =====================

        mesh.vertices[iVertex + 9] = prevPosition.x - dir.x + norm.x;
        mesh.vertices[iVertex + 10] = prevPosition.y - dir.y + norm.y;
        mesh.vertices[iVertex + 11] = 0.0f;

        mesh.normals[iNormal + 9] = -uSize;
        mesh.normals[iNormal + 10] = uSize;
        mesh.normals[iNormal + 11] = z;

        mesh.texcoords[iTexCoords + 6] = 1;
        mesh.texcoords[iTexCoords + 7] = 0;


        mesh.vertices[iVertex + 12] = nextPosition.x + dir.x - norm.x;
        mesh.vertices[iVertex + 13] = nextPosition.y + dir.y - norm.y;
        mesh.vertices[iVertex + 14] = 0.0f;

        mesh.normals[iNormal + 12] = z + uSize;
        mesh.normals[iNormal + 13] = -uSize;
        mesh.normals[iNormal + 14] = z;

        mesh.texcoords[iTexCoords + 8] = 0;
        mesh.texcoords[iTexCoords + 9] = 1;


        mesh.vertices[iVertex + 15] = nextPosition.x + dir.x + norm.x;
        mesh.vertices[iVertex + 16] = nextPosition.y + dir.y + norm.y;
        mesh.vertices[iVertex + 17] = 0.0f;

        mesh.normals[iNormal + 15] = z + uSize;
        mesh.normals[iNormal + 16] = uSize;
        mesh.normals[iNormal + 17] = z;

        mesh.texcoords[iTexCoords + 10] = 1;
        mesh.texcoords[iTexCoords + 11] = 1;
    }

    UploadMesh(&mesh, false);

    return mesh;
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

	InitWindow(screenWidth, screenHeight, "Oscilloscope Music");

	SetTargetFPS(targetFPS);

    AudioData audioData = CreateAudioData(DEFAULT_AUDIO_SAMPLES_COUNT, sampleRate);

    float deltaTime = 1.0f / (float)targetFPS;
    float accumulatedTime = 0.0f;

    FrameGeneratorState state;
    state.accumulatedTime = 0.0f;
    state.aspectRatio = (float)screenWidth / (float)screenHeight;

    while(accumulatedTime < animationTime)
    {
        GenerateAudioFrame(&state, &audioData, deltaTime);
        accumulatedTime += deltaTime;
    }

    Normalize01Wave(&audioData);

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
    SetShaderValue(shader, GetShaderLocation(shader, "aspectRatio"), &state.aspectRatio, SHADER_UNIFORM_FLOAT);

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