#include "gameplay.h"

u8* parse_word_guess(const char* guess, const char* solution)
{
    u8* result = (u8*)calloc(5, sizeof(char));
    assert(result != NULL);

    for (u32 i = 0; i < 5; i++) {
        char c[2] = {0};
        c[0] = guess[i];

        char* found = strpbrk(solution, c);

        if (found == NULL) {
            continue;
        }

        if (solution[i] == guess[i]) {
            result[i] = 2;
        } else {
            result[i] = 1;
        }
    }

    return result;
}

void print_parsed_guess(const char* guess, u8* parsedGuess)
{
    for (u32 i = 0; i < 5; i++) {
        printf("%s", colors[parsedGuess[i]]);
        printf("%c", guess[i]);
    }

    printf("\033[39m\n");
}