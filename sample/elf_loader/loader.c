#include "loader.h"
#include <sys/mman.h>

void elf_run(void *buf, char **argv, char **env) {
	// get elf file header
	Elf_Ehdr *hdr = (Elf_Ehdr *)buf;

	void *stack = mmap(0, STACK_SIZE, PROT_READ)
}