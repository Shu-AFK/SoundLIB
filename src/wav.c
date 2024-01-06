#include "WAV.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int is_little_endian() {
    int num = 1;
    // The least significant byte is at the lowest memory address in little-endian
    return (*(char*)&num == 1);
}

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

    for (int i = 0; i < size; i++)
        ret[i] = arr[i];

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

    // Calculates the expected chunk size while allowing for a certain tolerance if it is slightly different
    DWORD expected_chunk_size = 4 + (8 + header.subchunk1Size) + (8 + header.subchunk2Size);
    int tolerance = 200;
    if(abs(header.chunkSize - expected_chunk_size) > tolerance)  return 1;

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

void reverse_bytes(BYTE *array, DWORD size, int block_size)
{
    int is_little_endian_system = is_little_endian();

    // If the system is big endian there is no need to reverse the bytes
    if(is_little_endian_system)
    {
        for (int i = 0; i < size; i += block_size)
        {
            for (int j = 0; j < block_size / 2; j++)
            {
                int idx1 = i + j;
                int idx2 = i + block_size - j - 1;
                BYTE temp = array[idx1];
                array[idx1] = array[idx2];
                array[idx2] = temp;
            }
        }
    }
}

int get_block_size(WAVHEADER header)
{
    int block_size = header.numChannels * (header.bitsPerSample / 8);

    return block_size;
}

int reverse_file_wave(FILE *input, FILE *output)
{
    WAVHEADER *header = load_header(input);
    if(check_format_wave(*header) != 0)
    {
        perror("Input file is not a WAV file!\n");
        free(header);
        return 1;
    }

    if(fwrite(header, sizeof(WAVHEADER), 1, output) != 1)
    {
        perror("Could not write header to the output WAV file!\n");
        free(header);
        return 1;
    }

    BYTE file[header->subchunk2Size];
    if(fread(file, sizeof(BYTE), header->subchunk2Size, input) != header->subchunk2Size)
    {
        perror("Error in reading data to array!\n");
        free(header);
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
        perror("Content was not successfully written to the output file!");
        free(header);
        return 1;
    }

    free(header);
    return 0;
}

int change_vol_wave(FILE *input, FILE *output, double vol)
{
    return 1;
}