#include "json_utils.h"

char* json_get_value_by_key(const char* key, const char* json)
{
    u32 jsonLen = strlen(json);
    char* jsonCopy = (char*)malloc(jsonLen * sizeof(char) + 1);
    assert(jsonCopy != NULL);

    strncpy(jsonCopy, json, jsonLen + 1);

    char* delimiters = "{\":\",}";
    char* token = strtok(jsonCopy, delimiters);

    while (token != NULL && strcmp(token, key) != 0) {
        token = strtok(NULL, delimiters);
    };

    assert(token != NULL);
    token = strtok(NULL, delimiters);
    assert(token != NULL);

    u32 tokenLen = strlen(token);
    char* value = (char*)malloc(sizeof(char) * tokenLen + 1);

    for (u32 i = 0; i < tokenLen; ++i) {
        value[i] = token[i];
    }

    value[tokenLen] = '\0';

    free(jsonCopy);
    
    return value;
}
