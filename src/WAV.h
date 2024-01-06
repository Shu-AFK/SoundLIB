#ifndef SOUNDLIB_WAV_H
#define SOUNDLIB_WAV_H

#include <stdint.h>
#include <stdio.h>

// These data types are essentially aliases for C/C++ primitive data types.
// Adapted from http://msdn.microsoft.com/en-us/library/cc230309.aspx.
// See https://en.wikipedia.org/wiki/C_data_types#stdint.h for more on stdint.h.

typedef uint8_t   BYTE;
typedef uint16_t  WORD;
typedef uint32_t  DWORD;

// The WAVHEADER structure contains information about the type, size,
// and layout of a file that contains audio samples.
// Adapted from http://soundfile.sapp.org/doc/WaveFormat/.

typedef struct
{
    BYTE   chunkID[4];
    DWORD  chunkSize;
    BYTE   format[4];
    BYTE   subchunk1ID[4];
    DWORD  subchunk1Size;
    WORD   audioFormat;
    WORD   numChannels;
    DWORD  sampleRate;
    DWORD  byteRate;
    WORD   blockAlign;
    WORD   bitsPerSample;
    BYTE   subchunk2ID[4];
    DWORD  subchunk2Size;
} __attribute__((__packed__))
        WAVHEADER;

WAVHEADER *load_header(FILE *input);
int check_format_wave(WAVHEADER header);

int is_little_endian();
DWORD convert_endian(DWORD value);


// Functions used for reverse WAV
void reverse_bytes(BYTE *array, DWORD size, int block_size);
int get_block_size(WAVHEADER header);

int reverse_file_wave(FILE *input, FILE *output);
int change_vol_wave(FILE *input, FILE *output, double vol);
int change_speed_wave(FILE *input, FILE *output, double speed);

#endif //SOUNDLIB_WAV_H
