#ifndef STRING_H_
#define STRING_H_

#include "types.h"
#include <stdlib.h>

typedef struct {
    char* data;
    u32 length;
    u32 capacity;
} String;

String* create_string(u32 capacity);
void free_string(String* str);
void clear_string(String* str);

void fill_string_with_char(String* str, char c);
void grow_string_capacity(String* str, u32 factor);

#endif // STRING_H_
