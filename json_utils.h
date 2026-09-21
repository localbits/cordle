#ifndef JSON_UTILS_H_
#define JSON_UTILS_H_

#include "string.h"
#include <stdbool.h>
#include <assert.h>
#include <string.h>

String* json_get_value_by_key(const char* key, char* json);

#endif // JSON_UTILS_H_
