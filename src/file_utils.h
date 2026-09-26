#ifndef FILE_UTILS_H_
#define FILE_UTILS_H_

#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* write_file_into_buffer(const char* filePath, const char* openingMode);
char* pick_random_word_from_buffer(char* buffer, u32 seed);

#endif // FILE_UTILS_H_
