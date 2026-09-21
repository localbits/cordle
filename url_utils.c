#include "url_utils.h"

MemoryStruct init_mem_chunk(u32 capacity)
{
    MemoryStruct chunk = {0};   

    char* memory = (char*)malloc(capacity);
    assert(memory != NULL);

    chunk.memory = memory;
    chunk.size = 0;

    return chunk;
}

void free_mem_chunk(MemoryStruct* chunk)
{
    free(chunk->memory);
}

static u32 mem_callback(void* contents, u32 size, u32 nmemb, void* userData)
{
    u32 realSize = size * nmemb;
    MemoryStruct* mem = (MemoryStruct*)userData;

    mem->memory = realloc(mem->memory, mem->size + realSize + 1);
    assert(mem->memory != NULL);

    memcpy(&(mem->memory[mem->size]), contents, realSize);
    mem->size += realSize;
    mem->memory[mem->size] = 0;

    return realSize;
}

CURLcode make_curl_get_request(const char* url, MemoryStruct* chunk)
{
    CURL* curlHandle;
    CURLcode requestResult;

    curl_global_init(CURL_GLOBAL_ALL);
    curlHandle = curl_easy_init();

    curl_easy_setopt(curlHandle, CURLOPT_URL, url);

    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, mem_callback);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, (void*)chunk);

    curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    requestResult = curl_easy_perform(curlHandle);

    curl_easy_cleanup(curlHandle);
    curl_global_cleanup();

    return requestResult;
}

