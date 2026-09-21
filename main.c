#include <stdio.h>
#include "url_utils.h"
#include "json_utils.h"
#include "file_utils.h"

int main(void)
{
    const char* wordleUrl = "https://www.nytimes.com/svc/wordle/v2/2026-04-28.json";
    MemoryStruct chunk = init_mem_chunk(1);

    if (make_curl_get_request(wordleUrl, &chunk) != CURLE_OK) {
      fprintf(stderr, "Get request failed\n");
      return EXIT_FAILURE;
    }

    chunk.memory[chunk.size] = '\0';

    FILE* file = open_file("test.json", "rb");
    String* json = load_file_into_buffer(file);
    String* val = json_get_value_by_key("languages", json->data);

    printf("val: %s", val->data);

    free_mem_chunk(&chunk);

    return 0;
}
