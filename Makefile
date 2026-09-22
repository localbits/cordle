# 
# Cordle 
#

CC := gcc

PROGRAM_NAME := cordle
BUILD_DIR := src
OBJECT_DIR := $(BUILD_DIR)/obj

RELEASE_OBJECT_DIR := $(OBJECT_DIR)/release
DEBUG_OBJECT_DIR := $(OBJECT_DIR)/debug

CFLAGS := -Wall -Wextra
DEBUG_CFLAGS := $(CFLAGS) -g -fsanitize=address
DEBUG_LDFLAGS := -fsanitize=address

# Libraries
LDLIBS := -l curl

ifeq ($(OS),Windows_NT)
	EXE_EXT := .exe
	MKDIR_P := mkdir -p
	RM := rm -rf
else
	EXE_EXT :=
	MKDIR_P := mkdir -p
	RM := rm -rf
endif

PROGRAM := $(PROGRAM_NAME)$(EXE_EXT)
DEBUG_PROGRAM := $(PROGRAM_NAME)-debug$(EXE_EXT)

rwildcard = $(foreach dir,$(wildcard $1*/),$(call rwildcard,$(dir),$2)) \
            $(wildcard $1$2)

SRC := $(call rwildcard,$(BUILD_DIR)/,*.c)

RELEASE_OBJECTS := $(patsubst $(BUILD_DIR)/%.c,$(RELEASE_OBJECT_DIR)/%.o,$(SRC))
DEBUG_OBJECTS := $(patsubst $(BUILD_DIR)/%.c,$(DEBUG_OBJECT_DIR)/%.o,$(SRC))

.PHONY: all debug clean run clean_obj

all: $(PROGRAM)

debug: $(DEBUG_PROGRAM)

run: $(PROGRAM)
	./$(PROGRAM)

$(PROGRAM): $(RELEASE_OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(DEBUG_PROGRAM): $(DEBUG_OBJECTS)
	$(CC) $(DEBUG_LDFLAGS) $^ $(LDLIBS) -o $@

$(RELEASE_OBJECT_DIR)/%.o: $(BUILD_DIR)/%.c
	$(MKDIR_P) $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEBUG_OBJECT_DIR)/%.o: $(BUILD_DIR)/%.c
	$(MKDIR_P) $(@D)
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

clean_obj:
	$(RM) $(OBJECT_DIR)

clean:
	$(RM) $(OBJECT_DIR) $(PROGRAM) $(DEBUG_PROGRAM)
