#include "string.h"

String* create_string(u32 capacity)
{
    String* string = (String*)malloc(sizeof(String));
    if (string == NULL) {
        return NULL;
    }

    char* data = (char*)malloc(sizeof(char) * capacity + 1);
    if (data == NULL) {
        free(string);
        return NULL;
    }

    data[capacity] = '\0';

    string->data = data;
    string->capacity = capacity + 1;
    string->length = capacity;

    return string;
}

void free_string(String* str)
{
    free(str->data);
    free(str);
    str = NULL;
}

void clear_string(String* str)
{
    str->length = 0;
}

void fill_string_with_char(String* str, char c)
{
    for (u32 i = 0; i < str->length; ++i) {
        str->data[i] = c;
    }
}

void grow_string_capacity(String* str, u32 factor)
{
    u32 newCapacity = str->capacity * factor;
    str->data = (char*)realloc(str->data, newCapacity * sizeof(char));
    str->capacity = newCapacity;
}
