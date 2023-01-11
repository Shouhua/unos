CFLAGS := -g -o

.PHONY: clean all
all: elk entry_point hello

%: %.c
	gcc $< $(CFLAGS) $@
hello: hello.asm
	nasm -f elf64 $<
	ld $@.o -o $@
clean:
	rm *.o elk entry_point hello
