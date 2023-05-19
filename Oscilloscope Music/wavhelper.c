#include "wavhelper.h"

#include <raymath.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint16_t* CreateStereoWavSamples(const float* leftChannel, const float* rightChannel, const uint32_t samplesCount)
{
    uint16_t* samples = malloc(sizeof(uint16_t) * samplesCount * 2);

	for (int i = 0; i < samplesCount; i++)
	{
        samples[i << 1]       = Clamp((leftChannel[i] * 0.5 * (double)USHRT_MAX), SHRT_MIN, SHRT_MAX);
        samples[(i << 1) + 1] = Clamp((rightChannel[i] * 0.5 * (double)USHRT_MAX), SHRT_MIN, SHRT_MAX);
	}

    return samples;
}

void FreeWavSamples(uint16_t* wavSamples)
{
    free(wavSamples);
}

uint32_t CreateWav16PCMStereo(const uint16_t* samples, const uint32_t sampleRate, const uint32_t samplesCount, uint8_t** fullData)
{
    WaveHeader wav;
    memcpy(wav.riffHeader, "RIFF", sizeof(uint8_t) * 4);
    memcpy(wav.waveHeader, "WAVE", sizeof(uint8_t) * 4);

    uint16_t bitsPerSample = 16;

    memcpy(wav.fmtHeader, "fmt ", sizeof(uint8_t) * 4);
    wav.fmtChunkSize = 16;
    wav.audioFormat = 1;
    wav.numChannels = 2;
    wav.sampleRate = sampleRate;
    wav.byteRate = wav.sampleRate * bitsPerSample * wav.numChannels / 8;
    wav.sampleAlignment = (wav.numChannels * bitsPerSample) / 8;
    wav.bitDepth = bitsPerSample;

    memcpy(wav.dataHeader, "data", sizeof(uint8_t) * 4);
    wav.dataBytes = samplesCount * wav.numChannels * bitsPerSample / 8;

    static_assert(sizeof(WaveHeader) == 44, "");

    int dataSize = sizeof(WaveHeader) + wav.dataBytes;
    *fullData = malloc(dataSize);

    memcpy(*fullData, &wav, sizeof(WaveHeader));
    memcpy(*fullData + sizeof(WaveHeader), samples, wav.dataBytes);
    
    return wav.dataBytes;
}

void FreeWav(uint8_t* data)
{
    free(data);
}