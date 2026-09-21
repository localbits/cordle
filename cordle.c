#include <stdio.h>
#include "url_utils.h"
#include "json_utils.h"
#include "types.h"
#include "string.h"
#include "gameplay.h"

int main(void)
{
    const char* wordleUrl = "https://www.nytimes.com/svc/wordle/v2/2026-04-28.json";
    MemoryStruct chunk = init_mem_chunk(1);

    if (make_curl_get_request(wordleUrl, &chunk) != CURLE_OK) {
      fprintf(stderr, "Get request failed\n");
      return EXIT_FAILURE;
    }

    String* solution = json_get_value_by_key("solution", chunk.memory);

    const char* guess = "quack";
    u8* result = parse_word_guess(guess, solution->data);

    print_parsed_guess(guess, result);

    free_mem_chunk(&chunk);
    
    return 0;
}
