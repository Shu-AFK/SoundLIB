cmake_minimum_required(VERSION 3.27)
project(SoundLIB C)

set(CMAKE_C_STANDARD 23)

include_directories("C:\\Users\\Floyd\\Downloads\\mpg123-1.32.3-x86-64\\mpg123-1.32.3-x86-64")

add_executable(SoundLIB src/main.c src/WAV.h
        src/wav.c
        src/mp3.h
        src/mp3.c)

target_link_libraries(SoundLIB "C:\\Users\\Floyd\\Downloads\\mpg123-1.32.3-x86-64\\mpg123-1.32.3-x86-64\\libmpg123-0.dll")
