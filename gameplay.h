#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef enum {
    COLOR_RESET = 0,
    COLOR_CORRECT,
    COLOR_HINT,
    COLOR_COUNT
} Colors;

static const char* colors[COLOR_COUNT] = {
    "\033[39m",
    "\033[33m",
    "\033[32m"
};

u8* parse_word_guess(const char* guess, const char* solution);

void print_parsed_guess(const char* guess, u8* parsedGuess);

#endif // GAMEPLAY_H