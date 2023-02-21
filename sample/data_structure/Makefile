# SRC := $(shell find . -type f -name "*.c")
SRC := $(wildcard *.c)
# BIN := $(patsubst %.c, %, $(SRC))
BIN := $(SRC:.c=)
BUILD_DIR := build

.PHONY: all clear
all: $(BUILD_DIR) $(BIN)

$(BUILD_DIR):
	@mkdir build

%: %.c
	@gcc $< -g -o $(BUILD_DIR)/$@

clear:
	@rm -rf $(BUILD_DIR)
	@rm -f $(BIN)