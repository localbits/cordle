#include "json_utils.h"

String* json_get_value_by_key(const char* key, char* json)
{
    char* delimiters = "{\":\",}";
    char* token = strtok(json, delimiters);

    while (token != NULL && strcmp(token, key) != 0) {
        token = strtok(NULL, delimiters);
    };

    assert(token != NULL);
    token = strtok(NULL, delimiters);
    assert(token != NULL);

    u32 tokenLen = strlen(token);
    String* value = create_string(tokenLen);

    for (u32 i = 0; i < tokenLen; ++i) {
        value->data[i] = token[i];
    }

    return value;
}
