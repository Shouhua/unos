# SRC := $(shell find . -type f -name "*.c")
SRC := $(wildcard *.c)
# BIN := $(patsubst %.c, %, $(SRC))
BIN := $(SRC:.c=)

.PHONY: all clear
all: $(BIN)

%: %.c
	@gcc $< -g -o $@

clear:
	@rm -f $(BIN)