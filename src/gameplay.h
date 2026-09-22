#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "types.h"
#include "url_utils.h"
#include "json_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>

typedef struct {
    u8* items;
    u8 size;
} ParsedWordVector;

void game_loop(void);

#endif // GAMEPLAY_H
