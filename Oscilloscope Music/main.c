#include <corecrt_math.h>
#include <stdint.h>
#include <raylib.h>

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

        SetAudioSample(audioData, i, currentData, mask);
    }
}

Vector2 AudioToScreen(const float leftSample, const float rightSample, const uint32_t screenWidth, const uint32_t screenHeight)
{
    Vector2 res;

    const float aspectRatio = (float)screenWidth / (float)screenHeight;

    float newLeftSample = leftSample * 2.0f - 1.0f;
    newLeftSample /= aspectRatio;
    newLeftSample = (newLeftSample + 1.0f) * 0.5f;

    res.x = newLeftSample * (float)screenWidth;
    res.y = (1.0f - rightSample) * (float)screenHeight;
    
    return res;
}



int main(void)
{
	const uint32_t screenWidth   = 800;
	const uint32_t screenHeight  = 450;

    const int      targetFPS     = 60;

    const uint32_t sampleRate    = 48000;

          float    animationTime = 30.0f;

	InitWindow(screenWidth, screenHeight, "Oscilloscope Music");

	SetTargetFPS(targetFPS);

    AudioData audioData = CreateAudioData(DEFAULT_AUDIO_SAMPLES_COUNT, sampleRate);

    float deltaTime = 1.0f / (float)targetFPS;
    float accumulatedTime = 0.0f;

    FrameGeneratorState state;
    state.accumulatedTime = 0.0f;

    while(accumulatedTime < animationTime)
    {
        GenerateAudioFrame(&state, &audioData, deltaTime);
        accumulatedTime += deltaTime;
    }

    /*
    for (int i = 0; i < samplesCount; i++)
    {
        const float value = (1.0f + sinf(PI * 2.0f * ((float)i / 1000.0f))) * 0.5f;
        leftSamples[i] = value;
        rightSamples[i] = value;
    }*/

    //AddSineWave(&audioData, 0, 100000, LEFT_CHANNEL, 261.63f, 0.0f, 0.5f);
    //AddSineWave(&audioData, 0, 100000, RIGHT_CHANNEL, 261.63f, PI / 2.0f, 0.5f);
    

    Normalize01Wave(&audioData);

    animationTime = (float)audioData.samplesCount / (float)sampleRate;
    
    uint16_t* wavSamples = CreateStereoWavSamples(audioData.leftSamples, audioData.rightSamples, audioData.samplesCount);
    uint8_t* wavData;
    uint32_t wavDataLength = CreateWav16PCMStereo(wavSamples, sampleRate, audioData.samplesCount, &wavData);

    InitAudioDevice();
    Music music = LoadMusicStreamFromMemory(".wav", wavData, wavDataLength);
    PlayMusicStream(music);

    float previousMusicTimePlayed = 0.0f;

	while (!WindowShouldClose())
	{
        float currentMusicTimePlayed = GetMusicTimePlayed(music);

        if (currentMusicTimePlayed < previousMusicTimePlayed)
            previousMusicTimePlayed = 0.0f;

        UpdateMusicStream(music);

		BeginDrawing();

        ClearBackground(BLACK);

        int firstSampleIndex = (int)(previousMusicTimePlayed / animationTime * (float)audioData.samplesCount);
        int lastSampleIndex = (int)(currentMusicTimePlayed / animationTime * (float)audioData.samplesCount);

        for (int i = firstSampleIndex; i < lastSampleIndex - 1; i++)
        {
            float leftSample = audioData.leftSamples[i];
            float nextLeftSample = audioData.leftSamples[i + 1];

            float rightSample = audioData.rightSamples[i];
            float nextRightSample = audioData.rightSamples[i + 1];

            Vector2 prevPosition = AudioToScreen(leftSample, rightSample, screenWidth, screenHeight);
            Vector2 nextPosition = AudioToScreen(nextLeftSample, nextRightSample, screenWidth, screenHeight);

            DrawLine((int)prevPosition.x, (int)prevPosition.y, (int)nextPosition.x, (int)nextPosition.y, GREEN);
        }

		EndDrawing();

        previousMusicTimePlayed = currentMusicTimePlayed;
	}

    FreeWav(wavData);
    FreeWavSamples(wavSamples);

    FreeAudioData(&audioData);

	return 0;
}