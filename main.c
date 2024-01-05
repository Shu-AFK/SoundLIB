#include <stdio.h>
#include <mpg123.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include "WAV.h"

void print_help();
int get_file_format(char *file);

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
        printf("Invalid flag %s. To view possible flags use ./SoundLIB -h.\n", argv[1]);
        exit(1);
    }

    // Ensure that only one filter is specified
    if(getopt(argc, argv, flags) != -1)
    {
        printf("Only one flag is allowed!\n");
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
                printf("Incorrect use. Please refer to './SoundLIB -h' for more information!\n");
                exit(3);
            }
            strcpy(input_file_name, argv[2]);
            strcpy(output_file_name, argv[3]);

            // Checks if names got copied correctly
            if(input_file_name == nullptr || output_file_name == nullptr)
            {
                printf("Issue copying file names!\n");
                exit(-1);
            }

            break;

        case 'v':
        case 's':
            if(argc != 5)
            {
                printf("Incorrect use. Please refer to './SoundLIB -h' for more information!\n");
                exit(3);
            }
            strcpy(input_file_name, argv[2]);
            strcpy(output_file_name, argv[3]);

            // Checks if names got copied correctly
            if(input_file_name == nullptr || output_file_name == nullptr)
            {
                printf("Issue copying file names!\n");
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
        printf("File formats provided are not supported!\n");
        exit(4);
    }
    if(format_in != format_out)
    {
        printf("The file format of the input file is not equal to the output file!\n");
        exit(5);
    }

    // Open input and output files
    FILE *input = fopen(input_file_name, "r");
    FILE *output = fopen(output_file_name, "w");
    
    // If the file format is WAVE proceed with the specified flag
    if(format_in == 1)
    {
        switch (flag) {
            case 'r':
                if(reverse_file_wave(input, output) != 0)
                {
                    printf("Error in reversing wave file!\n");
                    goto cleanup_error;
                }
                break;

            case 'v':
                if(change_vol_wave(input, output, atof(argv[2])) != 0)
                {
                    printf("Error in changing volume of wave file!\n");
                    goto cleanup_error;
                }
                break;

            case 's':
                if(change_speed_wave(input, output, atof(argv[2])) != 0)
                {
                    printf("Error in changing playback speed of wave file!\n");
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
                    printf("Error in reversing mp3 file!\n");
                    goto cleanup_error;
                }
                break;

            case 'v':
                if(change_vol_mp3(input, output, atof(argv[2])) != 0)
                {
                    printf("Error in changing volume of mp3 file!\n");
                    goto cleanup_error;
                }
                break;

            case 's':
                if(change_speed_mp3(input, output, atof(argv[2])) != 0)
                {
                    printf("Error in changing playback speed of mp3 file!\n");
                    goto cleanup_error;
                }
                break;

            default:
                goto cleanup_error;
        }
    }

    exit(0);

cleanup_error:
    fclose(input);
    fclose(output);
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

    char *end_of_file = nullptr;
    strcpy(end_of_file, &file[beginning_of_format]);
    if(end_of_file == nullptr)
    {
        printf("Error in copying string (get_file_format())\n");
        exit(-1);
    }

    if(strcmp(end_of_file, ".wav") == 0 || strcmp(end_of_file, ".wave") == 0)
        return 1;
    else if(strcmp(end_of_file, ".mp3") == 0)
        return 2;
    else
        return 0;
}