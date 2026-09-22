#include "gameplay.h"

typedef enum {
    COLOR_RESET = 0,
    COLOR_CORRECT,
    COLOR_HINT,
    COLOR_COUNT
} Colors;

typedef enum {
    GAME_MODE_DAILY = 1,
    GAME_MODE_RANDOM,
    GAME_MODE_ARBITRARY,
    GAME_MODE_COUNT
} GameModes;

static const char* colors[COLOR_COUNT] = {
    "\033[0m",
    "\033[32m",
    "\033[33m",
};

static void flush_stdin(void)
{
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static ParsedWordVector create_parsed_word_vector(u8 size)
{
    ParsedWordVector vector = {0};

    u8* items = (u8*)calloc(size, sizeof(char));
    assert(items != NULL);

    vector.items = items;
    vector.size = size;

    return vector;
}

static s32 first_occurrence_in_array(char* s, char key)
{
    s32 occurrence = -1;
    u32 strLen = strlen(s);

    for (u32 i = 0; i < strLen; ++i) {
        if (s[i] == key) {
            occurrence = i;
            return occurrence;
        }
    }

    return -1;
}

static void free_parsed_word_vector(ParsedWordVector* vector)
{
    free(vector->items);
}

static u32 hash_char(char c)
{
    return tolower(c) - '0' - 49;
}

// TODO: Reimplement guess parsing logic
static void parse_word_guess(ParsedWordVector* vector, char* guess, char* solution)
{

}

static char* retrieve_daily_wordle_url(void)
{
    static char dailyUrl[256];

    time_t now = time(NULL);
    struct tm* currentTime = localtime(&now);

    char formattedDate[11];
    
    strftime(formattedDate, sizeof(formattedDate), "%Y-%m-%d", currentTime);

    snprintf(dailyUrl, sizeof(dailyUrl), "https://www.nytimes.com/svc/wordle/v2/%s.json", formattedDate);

    return dailyUrl;
}

static char* retrieve_arbitrary_wordle_url(u16 weekDay, u16 month, u16 year)
{
    static char dailyUrl[256];
    char formattedDate[11];

    struct tm* customTime = {0};
    customTime->tm_mday = weekDay;
    customTime->tm_mon = month;
    customTime->tm_year = year;

    strftime(formattedDate, sizeof(formattedDate), "%Y-%m-%d", customTime);
    snprintf(dailyUrl, sizeof(dailyUrl), "https://www.nytimes.com/svc/wordle/v2/%s.json", formattedDate);

    return dailyUrl;
}

// TODO: Fix date so that we dont get a wordle from 1984 
static char* retrieve_random_wordle_url(u32 seed)
{
    time_t now = time(NULL);
    struct tm* currentTime = localtime(&now);

    srand(seed);

    u32 rng = rand();

    u16 randomDay = rng % 32;
    u16 randomMonth = rng % 12;
    u16 randomYear = rng % (currentTime->tm_year + 1900);

    struct tm* randomTime = {0};
    randomTime->tm_mday = randomDay;
    randomTime->tm_year = randomYear;
    randomTime->tm_mon = randomMonth;
    
    static char dailyUrl[256];
    char formattedDate[11];
   
    strftime(formattedDate, sizeof(formattedDate), "%Y-%m-%d", randomTime);
    snprintf(dailyUrl, sizeof(dailyUrl), "https://www.nytimes.com/svc/wordle/v2/%s.json", formattedDate);

    return dailyUrl;
}

static u8* retrieve_user_arbitrary_date(void)
{
    static u8 arbitraryDate[3] = {0};
    printf("Enter desired day: \n");
    scanf("%c", &arbitraryDate[0]);

    printf("Enter desired month: \n");
    scanf("%c", &arbitraryDate[1]);

    printf("Enter desired year: \n");
    scanf("%c", &arbitraryDate[2]);

    return arbitraryDate;
}

static GameModes retrieve_user_game_mode(void)
{
    u32 gameMode;
    scanf("%d", &gameMode);
    return gameMode;
}

static char* retrieve_user_guess(void)
{
    static char guess[6] = {0};
    fgets(guess, sizeof(guess), stdin);
    flush_stdin();
    return guess;
}

static void clear_console(void)
{
#ifndef PLATFORM_WINDOWS
  system("clear");
#else
  system("cls");
#endif
}

static void print_parsed_guess(char* guess, ParsedWordVector* vector)
{
    for (u32 i = 0; i < vector->size; i++) {
        printf("%s", colors[vector->items[i]]);
        printf("%c ", toupper(guess[i]));
    }
    printf("%s\n", colors[COLOR_RESET]);
}

static void display_game_menu(void)
{
    printf("--- CORDLE: Wordle in C ---\n");
    printf("Pick a game mode:\n");
    printf("1: Daily\n");
    printf("2: Random\n");
    printf("3: Arbitrary date\n");
    printf("Enter your desired mode: ");
}

static void display_game_info(u16 remainingGuesses)
{
    printf("You have %d/6 remaining attempts left\n", remainingGuesses);
    printf("Type your guess: ");
}

static void display_game_over_screen(char* solution)
{
    printf("Game over\n");
    printf("The word was: %s", solution);
}

static void display_game_won_screen(void)
{
    printf("Congratulations you guessed the word!\n");
}

static bool game_won(char* solution, char* guess)
{
    const char* s = solution;
    const char* g = guess;
    return strcmp(s, g) == 0;
}

static bool game_lost(u8 remainingGuesses)
{
    return remainingGuesses <= 0;
}

static char* retrieve_game_mode_wordle_url(GameModes mode)
{
    char* url = NULL;
    u8* arbitraryDate = NULL;

    switch(mode) {
        case GAME_MODE_DAILY:
            url = retrieve_daily_wordle_url();
            break;
        case GAME_MODE_ARBITRARY:
            arbitraryDate = retrieve_user_arbitrary_date();
            url = retrieve_arbitrary_wordle_url(arbitraryDate[0], arbitraryDate[1], arbitraryDate[2]);
            break;
        case GAME_MODE_RANDOM:
            url = retrieve_random_wordle_url(time(NULL));
            break;
        case GAME_MODE_COUNT:
            break;
    }

    assert(url != NULL);
    return url;
}

static char* retrieve_wordle_solution(char* url)
{
    MemoryStruct chunk = init_mem_chunk(1);
    if (make_curl_get_request(url, &chunk) != CURLE_OK) {
        fprintf(stderr, "Failed to retrieve wordle solution\nn");
        return NULL;
    }

    char* solution = json_get_value_by_key("solution", chunk.memory);
    assert(solution != NULL);
    free_mem_chunk(&chunk);

    return solution;
}

GameModes game_menu(void)
{
    GameModes chosenMode;
    display_game_menu();
    chosenMode = retrieve_user_game_mode();

    while (chosenMode < 1 || chosenMode > 3) {
        printf("\nInvalid game option entered\n");
        display_game_menu();
        chosenMode = retrieve_user_game_mode();
    }

    clear_console();
    flush_stdin();

    return chosenMode;
}

void game_loop(void)
{
    GameModes mode = game_menu();
    ParsedWordVector vector = create_parsed_word_vector(5);
    u16 maxGuesses = 6;
    char* guess = NULL;
    char* url = retrieve_game_mode_wordle_url(mode);
    char* solution = retrieve_wordle_solution(url);

    do {
        display_game_info(maxGuesses);
        guess = retrieve_user_guess();
        parse_word_guess(&vector, guess, solution);
        print_parsed_guess(guess, &vector);
        maxGuesses--;
        if (game_lost(maxGuesses)) {
            display_game_over_screen(solution);
            return;
        }
    } while (!game_won(solution, guess));

    display_game_won_screen();
    free_parsed_word_vector(&vector);
    free(solution);
}
