#ifndef JSON_UTILS_H_
#define JSON_UTILS_H_

#include "string.h"
#include "types.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

char* json_get_value_by_key(const char* key, const char* json);

#endif // JSON_UTILS_H_
