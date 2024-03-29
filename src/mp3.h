#ifndef SOUNDLIB_MP3_H
#define SOUNDLIB_MP3_H

#include <stdio.h>

int reverse_file_mp3(FILE *input, FILE *output);
int change_vol_mp3(FILE *input, FILE *output, double vol);
int change_speed_mp3(FILE *input, FILE *output, double speed);
int mix_mp3(FILE *input, FILE *input2, FILE *output, float ratio);

#endif //SOUNDLIB_MP3_H
