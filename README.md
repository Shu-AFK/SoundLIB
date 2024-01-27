
# SoundLIB

> This project can be used to manipulate a given audio file, supporting mp3 and WAV files.
 
## Usage:

`./SoundLIB -flag [Optional input given the flag provided] input.mp3/wav output.mp3/wav`

## flags:

    -h prints help
    -r reverse
    -v change volume by value specified after the flag
    -s change playback speed
    -m mix two sound files together by a ratio specified

## flag specific usage:

> For mixing:
`./SoundLIB -m input1.mp3/wav input2.mp3/wav [Ratio] output.mp3/wav`
> The specified ration can only be between the values 0 and 1. The ratio specifies to what extent the first input file
> gets mixed into the output file.

## exit codes:  

    -1 Issue copying string
    1  Invalid flag specified
    2  Invalid amount of flags
    3  Incorrect specifications/Incorrect optional input given
    4  Incorrect file format
    5  Input and output file formats are not equal
    6  Issue executing the respective funtion
    7  Can't open input file
    8  Can't open output file
    9 Can't open input2 file
