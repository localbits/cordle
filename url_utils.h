#ifndef URL_UTILS_H_
#define URL_UTILS_H_

#include "types.h"
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
    char* memory;
    u32 size;
} MemoryStruct;

MemoryStruct init_mem_chunk(u32 capacity);
void free_mem_chunk(MemoryStruct* chunk);
CURLcode make_curl_get_request(const char* url, MemoryStruct* chunk);

#endif // URL_UTILS_H_
