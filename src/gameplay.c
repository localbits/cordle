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

static void reset_parsed_word_vector(ParsedWordVector* vector)
{
    for (u32 i = 0; i < vector->size; ++i) {
        vector->items[i] = COLOR_RESET;
    }
}

static void free_parsed_word_vector(ParsedWordVector* vector)
{
    free(vector->items);
}

static u8 hash_char(char c)
{
    return tolower(c) - '0' - 49;
}

static void parse_word_guess(ParsedWordVector* vector, char* guess, char* solution)
{
    reset_parsed_word_vector(vector);
    bool parsedLetters[26] = {false};

    u32 guessLength = strlen(guess);
    u32 solutionLength = strlen(solution);

    for (u32 i = 0; i < guessLength; ++i) {
        char c = guess[i];
        u8 guessIdx = hash_char(c);
        if (parsedLetters[guessIdx]) {
            continue;
        }

        parsedLetters[guessIdx] = true;

        for (u32 j = 0; j < solutionLength; ++j) {
            if (c == solution[j] && i == j) {
                vector->items[i] = COLOR_CORRECT;
            } else if (c == solution[j] && i != j) {
                vector->items[i] = COLOR_HINT;
            }
        }
    }
}

static char* retrieve_daily_wordle_url(void)
{
    static char url[256];

    time_t now = time(NULL);
    struct tm* currentTime = localtime(&now);

    char formattedDate[11];
    
    strftime(formattedDate, sizeof(formattedDate), "%Y-%m-%d", currentTime);

    snprintf(url, sizeof(url), "https://www.nytimes.com/svc/wordle/v2/%s.json", formattedDate);

    return url;
}

static char* retrieve_arbitrary_wordle_url(u16 weekDay, u16 month, u16 year)
{
    static char url[256];
    char formattedDate[11];

    struct tm* customTime = {0};
    customTime->tm_mday = weekDay;
    customTime->tm_mon = month;
    customTime->tm_year = year;

    strftime(formattedDate, sizeof(formattedDate), "%Y-%m-%d", customTime);
    snprintf(url, sizeof(url), "https://www.nytimes.com/svc/wordle/v2/%s.json", formattedDate);

    return url;
}

static u32 days_to_seconds(u32 days)
{
    return days * (60 * 60 * 24);
}

static u32 seconds_to_days(u32 seconds)
{
    return seconds / (60 * 60 * 24);
}

static struct tm get_random_wordle_time(u32 seed)
{
    srand(seed);

    time_t now = time(NULL);

    struct tm startingWordleTime = {0};
    startingWordleTime.tm_mday = 6;
    startingWordleTime.tm_year = 2021 - 1900;
    startingWordleTime.tm_wday = 19;

    time_t startingTime = mktime(&startingWordleTime);
    
    const u32 secondsElapsed = (u32)difftime(now, startingTime);
    const u32 elapsedDays = seconds_to_days(secondsElapsed) + 1;
    const u32 randomNumber = rand();
    const u32 randomDayIdx = randomNumber % elapsedDays;

    const time_t randomTime = startingTime + days_to_seconds(randomDayIdx);

    struct tm randomWordleTime = {0};
    localtime_r(&randomTime, &randomWordleTime);

    return randomWordleTime;
}

static char* retrieve_random_wordle_url(u32 seed)
{
    struct tm randomWordleTime = get_random_wordle_time(seed);
    static char url[256] = {0};
    char formattedDate[11] = {0};

    strftime(formattedDate, sizeof(formattedDate), "%Y-%m-%d", &randomWordleTime);
    snprintf(url, sizeof(url), "https://www.nytimes.com/svc/wordle/v2/%s.json", formattedDate);

    return url;
}

static u16* retrieve_user_arbitrary_date(void)
{
    static u16 arbitraryDate[3] = {0};
    printf("Enter desired day: \n");
    scanf("%" SCNd16,  &arbitraryDate[0]);

    printf("Enter desired month: \n");
    scanf("%" SCNd16,  &arbitraryDate[1]);

    printf("Enter desired year: \n");
    scanf("%" SCNd16,  &arbitraryDate[2]);

    return arbitraryDate;
}

static u32 retrieve_user_game_mode(void)
{
    u32 mode;
    scanf("%d", &mode);
    return mode;
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

static void display_wordle_date_info(char* json)
{
    char* date = json_get_value_by_key("print_date", json);
    printf("--- Playing wordle from: %s ---\n", date);
    free(date);
}

static void display_game_info(u16 remainingGuesses)
{
    printf("You have %d/6 remaining attempts left\n", remainingGuesses);
    printf("Type your guess: ");
}

static void display_game_over_screen(char* solution)
{
    printf("Game over\n");
    printf("The word was: %s\n", solution);
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
    u16* arbitraryDate = NULL;

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

static char* retrieve_wordle_json(const char* url)
{
    MemoryStruct chunk = init_mem_chunk(1);

    if (make_curl_get_request(url, &chunk) != CURLE_OK) {
        free_mem_chunk(&chunk);
        fprintf(stderr, "Failed to retrieve wordle solution\nn");
        return NULL;
    }

    static char json[256] = {0};
    strncpy(json, chunk.memory, sizeof(json));

    free_mem_chunk(&chunk);

    return json;
}

static char* retrieve_wordle_solution(char* json)
{
    char* solution = json_get_value_by_key("solution", json);
    assert(solution != NULL);
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
    char* json = retrieve_wordle_json(url);
    char* solution = retrieve_wordle_solution(json);

    display_wordle_date_info(json);

    while (maxGuesses > 0) {
        display_game_info(maxGuesses);

        guess = retrieve_user_guess();
        parse_word_guess(&vector, guess, solution);
        print_parsed_guess(guess, &vector);

        if (game_won(solution, guess)) {
            display_game_won_screen();
            break;
        }

        maxGuesses--;

        if (game_lost(maxGuesses)) {
            display_game_over_screen(solution);
            break;
        }
    }

    free_parsed_word_vector(&vector);
    free(solution);
}
