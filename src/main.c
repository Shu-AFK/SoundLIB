// TODO: Other functions besides reverse WAV
// TODO: Check if there is an issue in importing the WAV header

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include "WAV.h"
#include "mp3.h"

// General functions
void print_help();
int get_file_format(char *file);

char *copy_string(const char *source);

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