#include "WAV.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

WAVHEADER *load_header(FILE *input)
{
    WAVHEADER *header = malloc(sizeof(WAVHEADER));

    // Read the contents into the header object
    fread(&header->chunkID, sizeof(BYTE), 4, input);
    fread(&header->chunkSize, sizeof(DWORD), 1, input);
    fread(&header->format, sizeof(BYTE), 4, input);
    fread(&header->subchunk1ID, sizeof(BYTE), 4, input);
    fread(&header->subchunk1Size, sizeof(DWORD), 1, input);
    fread(&header->audioFormat, sizeof(WORD), 1, input);
    fread(&header->numChannels, sizeof(WORD), 1, input);
    fread(&header->sampleRate, sizeof(DWORD), 1, input);
    fread(&header->byteRate, sizeof(DWORD), 1, input);
    fread(&header->blockAlign, sizeof(WORD), 1, input);
    fread(&header->bitsPerSample, sizeof(WORD), 1, input);
    fread(&header->subchunk2ID, sizeof(BYTE), 4, input);
    fread(&header->subchunk2Size, sizeof(DWORD), 1, input);

    return header;
}

char *convert_byte_to_char(BYTE arr[], int size) {
    char *ret = malloc((size + 1) * sizeof(char));
    memcpy(ret, arr, size);
    ret[size] = '\0';

    return ret;
}

int check_format_wave(WAVHEADER header)
{
    // Creating the strings to compare
    char *chunkID = convert_byte_to_char(header.chunkID, 4);
    char *format = convert_byte_to_char(header.format, 4);
    char *subchunk1ID = convert_byte_to_char(header.subchunk1ID, 4);
    char *subchunk2ID = convert_byte_to_char(header.subchunk2ID, 4);

    // Checking for the right format in the first block
    if(strcmp(chunkID, "RIFF") != 0)     return 1;
    if(36 + header.subchunk2Size != header.chunkSize)  return 1;
    if(strcmp(format, "WAVE") != 0)      return 1;

    // Checking for the right format in the second block
    if(strcmp(subchunk1ID, "fmt ") != 0) return 1;

    // Checking for the right format in the third block
    if(strcmp(subchunk2ID, "data") != 0) return 1;

    // Free the allocated memory
    free(chunkID);
    free(format);
    free(subchunk1ID);
    free(subchunk2ID);

    return 0;
}

void reverse_bytes(BYTE *array, int size, int block_size)
{
    for (int i = 0; i < size; i += block_size)
    {
        for (int j = 0; j < block_size / 2; j++)
        {
            int idx1 = i + j;
            int idx2 = i + block_size - j - 1;
            swap(&array[idx1], &array[idx2], sizeof(BYTE));
        }
    }
}

int get_block_size(WAVHEADER header)
{
    int block_size = header.numChannels * (header.bitsPerSample / 8);

    return block_size;
}

void swap(BYTE *first, BYTE *second, int size)
{
    BYTE *buffer = malloc(size);
    memcpy(buffer, first, size);
    memcpy(first, second, size);
    memcpy(second, buffer, size);
    free(buffer);
}

int reverse_file_wave(FILE *input, FILE *output)
{
    WAVHEADER *header = load_header(input);
    if(check_format_wave(*header) != 0)
    {
        fprintf(stderr, "Input file is not a WAV file!\n");
        free(header);
        return 1;
    }

    if(fwrite(&header, sizeof(WAVHEADER), 1, output) != 1)
    {
        fprintf(stderr, "Could not write header to the output WAV file!\n");
        return 1;
    }

    BYTE file[header->subchunk2Size];
    if(fread(file, sizeof(BYTE), header->subchunk2Size, input) != header->subchunk2Size)
    {
        fprintf(stderr, "Error in reading data to array!\n");
        return 1;
    }

    BYTE backwards_file[header->subchunk2Size];

    int counter = 0;
    for(int i = header->subchunk2Size - 1; i >= 0; i--, counter++)
    {
        backwards_file[counter] = file[i];
    }

    reverse_bytes(backwards_file, header->subchunk2Size, get_block_size(*header));

    // Write the reverse array to the output file
    if(fwrite(backwards_file, 1, header->subchunk2Size, output) != header->subchunk2Size)
    {
        fprintf(stderr, "Content was not successfully written to the output file!\n");
        return 1;
    }

    free(header);
    return 0;
}