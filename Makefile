CC = gcc
MKDIR = mkdir -p
RMDIR = rm -rf
CFLAGS = -Wall -std=c99

SRC_DIR = ./src
INCLUDE_DIR = ./include
SOURCES := $(wildcard $(SRC_DIR)/*.c)

RELEASE_DIR = ./bin/release
RELEASE_OBJECTS := $(SOURCES:$(SRC_DIR)/%.c=$(RELEASE_DIR)/%.o)
RELEASE_EXECUTABLE = $(RELEASE_DIR)/simulator

DEBUG_DIR = ./bin/debug
DEBUG_OBJECTS := $(SOURCES:$(SRC_DIR)/%.c=$(DEBUG_DIR)/%.o)
DEBUG_EXECUTABLE = $(DEBUG_DIR)/simulator

all: release

release: CFLAGS += -O3
release: $(RELEASE_EXECUTABLE)

$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.c
	$(MKDIR) $(RELEASE_DIR)
	$(CC) -I $(INCLUDE_DIR) $(CFLAGS) -c $< -o $@

$(RELEASE_EXECUTABLE): $(RELEASE_OBJECTS)
	$(CC) $^ -o $@

debug: CFLAGS += -g
debug: $(DEBUG_EXECUTABLE)

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.c
	$(MKDIR) $(DEBUG_DIR)
	$(CC) -I $(INCLUDE_DIR) $(CFLAGS) -c $< -o $@

$(DEBUG_EXECUTABLE): $(DEBUG_OBJECTS)
	gcc $^ -o $@
	@echo

.PHONY: run
run: $(DEBUG_EXECUTABLE)
	@$(DEBUG_EXECUTABLE) $(ARGS) ||:

.PHONY: clean_debug
clean_debug:
	$(RMDIR) $(DEBUG_DIR)

.PHONY: clean
clean:
	$(RMDIR) $(RELEASE_DIR)
