#include <stdio.h>
#include <mpg123.h>
#include <getopt.h>
#include <stdlib.h>
#include "WAV.h"

void print_help();

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
    switch (flag) {
        case 'h':
            print_help();
            goto end;

        case 'r':
            if(argc != 4)
            {
                printf("Incorrect use. Please refer to './SoundLIB -h' for more information!\n");
                exit(3);
            }
            break;

        case 'v':
        case 's':
            if(argc != 5)
            {
                printf("Incorrect use. Please refer to './SoundLIB -h' for more information!\n");
                exit(3);
            }
            break;

        default:
            break;
    }

end:
    exit(0);
}

void print_help()
{
    printf("Usage: ./SoundLIB -flag [Optional input given the flag provided] input.mp3/wav output.mp3/wav\n\n");
    printf("Flags: -h prints help\n"
           "    -r reverse\n"
           "    -v change volume by value specified after the flag\n"
           "    -s change playback speed\n");
}
