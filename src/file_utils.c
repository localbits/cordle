#include "file_utils.h"

#define WORD_LENGTH 5

char* write_file_into_buffer(const char* filePath, const char* openingMode)
{
    FILE* file = fopen(filePath, openingMode);
    if (file == NULL) {
        fprintf(stderr, "Failed to open file at: %s\n", filePath);
        return NULL;
    }

    size_t fileByteSize = 0;
    fseek(file, 0, SEEK_END);
    fileByteSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(sizeof(*buffer) * fileByteSize + 1);
    buffer[fileByteSize] = '\0';

    fread(buffer, sizeof(char), fileByteSize, file);

    return buffer;
}

static u32 word_count_in_buffer(char* buffer)
{
    size_t bufferSize = strlen(buffer);

    u32 wordCount = 0;
    for (u32 i = 0; i < bufferSize; ++i) {
        if (buffer[i] == '\n') {
            wordCount++;
        }
    }

    if (buffer[bufferSize - 1] != '\n') {
        wordCount++;
    }
    
    return wordCount;
}

char* pick_random_word_from_buffer(char* buffer, u32 seed)
{
    static char word[WORD_LENGTH] = {0};
    srand(seed);

    u32 wordCount = word_count_in_buffer(buffer);

    size_t bufferSize = strlen(buffer);
    size_t randomWordNumber = rand() % wordCount + 1;

    u32 wordsRead = 0;

    u32 rightPtr = 0;

    while (rightPtr < bufferSize && wordsRead != randomWordNumber - 1) {
        if (buffer[rightPtr] == '\n') {
            wordsRead++;
        }
        rightPtr++;
    }

    for (u32 i = rightPtr, j = 0; buffer[i] != '\n'; ++i, ++j) {
        word[j] = buffer[i];
    }

    return word;
}
