// TODO: Other functions besides reverse WAV
// TODO: Check if there is an issue in importing the WAV header

#include <stdio.h>
#include <mpg123.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include "WAV.h"

// General functions
void print_help();
int get_file_format(char *file);

char *copy_string(const char *source);

// Functions used for WAV files
WAVHEADER *load_header(FILE *input);
int check_format_wave(WAVHEADER header);

// Functions used for reverse WAV
char *convert_byte_to_char(BYTE arr[], int size);
void reverse_bytes(BYTE *array, int size, int block_size);
int get_block_size(WAVHEADER header);
void swap(BYTE *first, BYTE *second, int size);

int reverse_file_wave(FILE *input, FILE *output);
int change_vol_wave(FILE *input, FILE *output, double vol);
int change_speed_wave(FILE *input, FILE *output, double speed);

int reverse_file_mp3(FILE *input, FILE *output);
int change_vol_mp3(FILE *input, FILE *output, double vol);
int change_speed_mp3(FILE *input, FILE *output, double speed);

int main(int argc, char **argv) {
    // defines the allowed flags
    char *flags = "hrvs";

    // Gets the flag specified and makes sure its only one flag
    char flag = getopt(argc, argv, flags);
    if(flag == '?')
    {
        fprintf(stderr, "Invalid flag %s. To view possible flags use ./SoundLIB -h.\n", argv[1]);
        exit(1);
    }

    // Ensure that only one filter is specified
    if(getopt(argc, argv, flags) != -1)
    {
        fprintf(stderr, "Only one flag is allowed!\n");
        exit(2);
    }

    // Checks for correct usage
    char *input_file_name = nullptr;
    char *output_file_name = nullptr;

    switch (flag) {
        case 'h':
            print_help();
            exit(0);

        case 'r':
            if(argc != 4)
            {
                fprintf(stderr, "Incorrect use. Please refer to './SoundLIB -h' for more information!\n");
                exit(3);
            }
            // Assign input and output file name their values while making sure it worked
            input_file_name = copy_string(argv[2]);
            if(input_file_name == nullptr)
            {
                fprintf(stderr, "Could not copy argv[2] into input_file_name!\n");
                exit(-1);
            }
            output_file_name = copy_string(argv[3]);
            if(output_file_name == nullptr)
            {
                fprintf(stderr, "Could not copy argv[3] into output_file_name!\n");
                free(input_file_name);
                exit(-1);
            }

            break;

        case 'v':
        case 's':
            if(argc != 5)
            {
                fprintf(stderr, "Incorrect use. Please refer to './SoundLIB -h' for more information!\n");
                exit(3);
            }

            // Assign input and output file name their values while making sure it worked
            input_file_name = copy_string(argv[3]);
            if(input_file_name == nullptr)
            {
                fprintf(stderr, "Could not copy argv[3] into input_file_name!\n");
                exit(-1);
            }
            output_file_name = copy_string(argv[4]);
            if(output_file_name == nullptr)
            {
                fprintf(stderr, "Could not copy argv[4] into output_file_name!\n");
                free(input_file_name);
                exit(-1);
            }

            break;

        default:
            break;
    }

    // Open the files and proceed, based on the format provided by the user
    int format_in = get_file_format(input_file_name); // 1 if WAV, 2 if mp3, 0 if invalid
    int format_out = get_file_format(output_file_name); // 1 if WAV, 2 if mp3, 0 if invalid

    // Checks if the file formats are valid
    if(format_out == 0 || format_in == 0)
    {
        fprintf(stderr, "File formats provided are not supported!\n");
        free(input_file_name);
        free(output_file_name);
        exit(4);
    }
    if(format_in != format_out)
    {
        fprintf(stderr, "The file format of the input file is not equal to the output file!\n");
        free(input_file_name);
        free(output_file_name);
        exit(5);
    }

    // Open input and output files
    FILE *input = fopen(input_file_name, "r");
    if(input == NULL)
    {
        fprintf(stderr, "Error in opening %s!\n", input_file_name);
        free(input_file_name);
        free(output_file_name);
        exit(7);
    }

    FILE *output = fopen(output_file_name, "w");
    if(output == NULL)
    {
        fprintf(stderr, "Error in opening %s!\n", output_file_name);
        fclose(input);
        free(input_file_name);
        free(output_file_name);
        exit(8);
    }
    
    // If the file format is WAVE proceed with the specified flag
    if(format_in == 1)
    {
        switch (flag) {
            case 'r':
                if(reverse_file_wave(input, output) != 0)
                {
                    fprintf(stderr, "Error in reversing wave file!\n");
                    goto cleanup_error;
                }
                break;

            case 'v':
                if(change_vol_wave(input, output, atof(argv[2])) != 0)
                {
                    fprintf(stderr, "Error in changing volume of wave file!\n");
                    goto cleanup_error;
                }
                break;

            case 's':
                if(change_speed_wave(input, output, atof(argv[2])) != 0)
                {
                    fprintf(stderr, "Error in changing playback speed of wave file!\n");
                    goto cleanup_error;
                }
                break;

            default:
                goto cleanup_error;
        }
    }
    
    // If the file format is mp3 proceed with the specified flag
    else
    {
        switch (flag) {
            case 'r':
                if(reverse_file_mp3(input, output) != 0)
                {
                    fprintf(stderr, "Error in reversing mp3 file!\n");
                    goto cleanup_error;
                }
                break;

            case 'v':
                if(change_vol_mp3(input, output, atof(argv[2])) != 0)
                {
                    fprintf(stderr, "Error in changing volume of mp3 file!\n");
                    goto cleanup_error;
                }
                break;

            case 's':
                if(change_speed_mp3(input, output, atof(argv[2])) != 0)
                {
                    fprintf(stderr, "Error in changing playback speed of mp3 file!\n");
                    goto cleanup_error;
                }
                break;

            default:
                goto cleanup_error;
        }
    }

    fclose(input);
    fclose(output);
    free(input_file_name);
    free(output_file_name);
    exit(0);

cleanup_error:
    fclose(input);
    fclose(output);
    free(input_file_name);
    free(output_file_name);

    exit(6);
}

void print_help()
{
    printf("Usage: ./SoundLIB -flag [Optional input given the flag provided] input.mp3/wav output.mp3/wav\n\n");
    printf("Flags: -h prints help\n"
           "    -r reverse\n"
           "    -v change volume by value specified after the flag\n"
           "    -s change playback speed\n");
}

int get_file_format(char *file)
{
    size_t beginning_of_format = strlen(file) - 4;
    if(file[beginning_of_format] != '.')
        beginning_of_format--;
    if(file[beginning_of_format] != '.')
        return 0;

    char *end_of_file = copy_string(&file[beginning_of_format]);
    if(end_of_file == nullptr)
    {
        fprintf(stderr, "Error in copying string (get_file_format())\n");
        exit(-1);
    }

    if(strcmp(end_of_file, ".wav") == 0 || strcmp(end_of_file, ".wave") == 0)
    {
        free(end_of_file);
        return 1;
    }
    else if(strcmp(end_of_file, ".mp3") == 0)
    {
        free(end_of_file);
        return 2;
    }
    else
    {
        free(end_of_file);
        return 0;
    }
}

char *copy_string(const char *source)
{
    // Checking for valid input
    if(source == nullptr)
    {
        fprintf(stderr, "Invalid input!\n");
        return nullptr;
    }

    //Allocate memory for the new string
    char *dest = (char *) malloc(strlen(source) + 1);

    // Checking if the memory allocation was successful
    if(dest != nullptr)
        strcpy(dest, source);
    else
    {
        fprintf(stderr, "Memory allocation failed!\n");
        return nullptr;
    }

    return dest;
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

char *convert_byte_to_char(BYTE arr[], int size) {
    char *ret = malloc((size + 1) * sizeof(char));
    memcpy(ret, arr, size);
    ret[size] = '\0';

    return ret;
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

int change_vol_wave(FILE *input, FILE *output, double vol)
{
    return 1;
}

int change_speed_wave(FILE *input, FILE *output, double speed)
{
    return 1;
}


int reverse_file_mp3(FILE *input, FILE *output)
{
    return 1;
}

int change_vol_mp3(FILE *input, FILE *output, double vol)
{
    return 1;
}

int change_speed_mp3(FILE *input, FILE *output, double speed)
{
    return 1;
}