#include "gameplay.h"

#define WORD_LENGTH 5
#define MAX_GUESSES 69

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

static void clear_console(void)
{
  system("clear");
}

static u8 hash_char(char c)
{
    return tolower(c) - 'a';
}

static void char_frequency_in_word(char* word, u8* vector)
{
    for (u32 i = 0; i < WORD_LENGTH; ++i) {
        u8 count = 0;
        u8 charIdx = hash_char(word[i]);
        for (u32 j = 0; j < WORD_LENGTH; ++j) {
            if (word[i] == word[j]) {
                count++;
            }
        }
        vector[charIdx] = count;
    }
}

static void reset_parsed_word_vector(u8* vector)
{
    memset(vector, COLOR_RESET, WORD_LENGTH);
}

static void parse_word_guess(u8* parsedWordVector, char* guess, char* solution)
{
    reset_parsed_word_vector(parsedWordVector);

    u8 frequencySolution[26] = {0};
    u8 parsedGuessLetters[26] = {0};

    char_frequency_in_word(solution, frequencySolution);

    for (u32 i = 0; i < WORD_LENGTH; ++i) {
        u8 charIdx = hash_char(guess[i]);
        if (guess[i] == solution[i]) {
            parsedWordVector[i] = COLOR_CORRECT;
            parsedGuessLetters[charIdx]++;
        }
    }

    for (u32 i = 0; i < WORD_LENGTH; ++i) {
        u8 charIdx = hash_char(guess[i]);
        if (parsedGuessLetters[charIdx] >= frequencySolution[charIdx]) {
            continue;
        }

        for (u32 j = 0; j < WORD_LENGTH; ++j) {
            if (guess[i] == solution[j] && parsedWordVector[i] == COLOR_RESET) {
                parsedGuessLetters[charIdx]++;
                parsedWordVector[i] = COLOR_HINT;
            }
        }
    }
}

static char* get_formatted_wordle_url(struct tm time)
{
    static char url[256];
    char date[11] = {0};

    strftime(date, sizeof(date), "%Y-%m-%d", &time);
    snprintf(url, sizeof(url), "https://www.nytimes.com/svc/wordle/v2/%s.json", date);

    return url;
}

static char* retrieve_daily_wordle_url(void)
{
    time_t now = time(NULL);
    struct tm* currentTime = localtime(&now);

    char* url = get_formatted_wordle_url(*currentTime);

    return url;
}

static char* retrieve_arbitrary_wordle_url(u16 weekDay, u16 month, u16 year)
{
    struct tm customTime = {0};
    customTime.tm_mday = weekDay;
    customTime.tm_mon = month - 1;
    customTime.tm_year = year - 1900;

    char* url = get_formatted_wordle_url(customTime);

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

static struct tm get_starting_wordle_time(void)
{
    return (struct tm){
        .tm_mday = 6,
        .tm_year = 2021 - 1900,                     
        .tm_wday = 19
    };
}

static struct tm get_random_wordle_time(u32 seed)
{
    srand(seed);

    time_t now = time(NULL);

    struct tm startingWordleTime = get_starting_wordle_time();

    time_t startingTime = mktime(&startingWordleTime);
    
    const u32 secondsElapsed = (u32)difftime(now, startingTime);
    const u32 elapsedDays = seconds_to_days(secondsElapsed) + 1;
    const u32 randomNumber = rand();
    const u32 randomDayIdx = randomNumber % elapsedDays;

    const time_t randomTime = startingTime + days_to_seconds(randomDayIdx);

    struct tm randomWordleTime = {0};
    #ifndef _WIN32
        localtime_r(&randomTime, &randomWordleTime);
    #else 
        localtime_s(&randomWordleTime, &randomTime);
    #endif
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

static bool is_arbitrary_date_valid(u16* date)
{
    struct tm startingWordleTime = get_starting_wordle_time();

    struct tm arbitraryDateTime = {0};
    arbitraryDateTime.tm_mday = date[0];
    arbitraryDateTime.tm_mon = date[1];
    arbitraryDateTime.tm_year = date[2] - 1900;

    time_t wordleStart = mktime(&startingWordleTime);
    time_t ArbitraryDateStart = mktime(&arbitraryDateTime);

    s32 offset = (s32)difftime(ArbitraryDateStart, wordleStart);

    return offset >= 0;
}

static u16* retrieve_user_arbitrary_date(void)
{
    static u16 arbitraryDate[3] = {0};
    while (true) {
        printf("Enter desired day: ");
        scanf("%" SCNd16,  &arbitraryDate[0]);

        printf("Enter desired month: ");
        scanf("%" SCNd16,  &arbitraryDate[1]);

        printf("Enter desired year: ");
        scanf("%" SCNd16,  &arbitraryDate[2]);

        if (is_arbitrary_date_valid(arbitraryDate)) {
            clear_console();
            break;
        }

        clear_console();
        printf("Invalid date entered, provide a date within starting range of: 19 Jun 2021\n");
    }

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
    static char guess[WORD_LENGTH + 1] = {0};
    fgets(guess, sizeof(guess), stdin);
    flush_stdin();
    return guess;
}

static void print_parsed_guess(char* guess, u8* parsedWordvector)
{
    for (u32 i = 0; i < WORD_LENGTH; i++) {
        printf("%s", colors[parsedWordvector[i]]);
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
    printf("You have %d/%d remaining attempts left\n", remainingGuesses, MAX_GUESSES);
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
            flush_stdin();
            clear_console();
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
    u8 vector[WORD_LENGTH] = {0};
    u16 maxGuesses = MAX_GUESSES;
    char* guess = NULL;
    char* url = retrieve_game_mode_wordle_url(mode);
    char* json = retrieve_wordle_json(url);
    char* solution = retrieve_wordle_solution(json);

    display_wordle_date_info(json);

    while (maxGuesses > 0) {
        display_game_info(maxGuesses);

        guess = retrieve_user_guess();
        parse_word_guess(vector, guess, solution);
        print_parsed_guess(guess, vector);

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

    free(solution);
}
