#pragma once

#include <stdint.h>

typedef struct Waveheader
{
    // RIFF Header
    uint8_t riffHeader[4]; // Contains "RIFF"
    uint32_t wavSize; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    uint8_t waveHeader[4]; // Contains "WAVE"

    // Format Header
    uint8_t fmtHeader[4]; // Contains "fmt " (includes trailing space)
    uint32_t fmtChunkSize; // Should be 16 for PCM
    uint16_t audioFormat; // Should be 1 for PCM. 3 for IEEE Float
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate; // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
    uint16_t sampleAlignment; // num_channels * Bytes Per Sample
    uint16_t bitDepth; // Number of bits per sample

    // Data
    uint8_t dataHeader[4]; // Contains "data"
    uint32_t dataBytes; // Number of bytes in data. Number of samples * num_channels * sample byte size
    // uint8_t bytes[]; // Remainder of wave file is bytes


} WaveHeader;

uint16_t* CreateStereoWavSamples(const float*, const float*, uint32_t);
void FreeWavSamples(uint16_t*);

uint32_t CreateWav16PCMStereo(const uint16_t*, uint32_t, uint32_t, uint8_t**);
void FreeWav(uint8_t*);