#ifndef __LOADER__
#define __LOADER__

#include <elf.h>

#define STACK_SIZE 20*1024*1024

#if INTPTR_MAX == INT32_MAX // 64bits
	typedef Elf32_Ehdr	Elf_Ehdr;
	typedef Elf32_Phdr	Elf_Phdr;
	typedef Elf32_Sym	Elf_Sym;
	typedef Elf32_Shdr	Elf_Shdr;
	typedef Elf32_Rel	Elf_Rel;
	typedef Elf32_Word	Elf_Word;
	#define	ELF_R_SYM(x) ELF32_R_SYM(x)
	#define	ELF_R_TYPE(x) ELF32_R_TYPE(x)
#elif INTPTR_MAX == INT64_MAX
	typedef Elf64_Ehdr	Elf_Ehdr;
	typedef Elf64_Phdr	Elf_Phdr;
	typedef Elf64_Sym	Elf_Sym;
	typedef Elf64_Shdr	Elf_Shdr;
	typedef Elf64_Rel	Elf_Rel;
	typedef Elf64_Word	Elf_Word;
	#define	ELF_R_SYM(x) ELF64_R_SYM(x)
	#define	ELF_R_TYPE(x) ELF64_R_TYPE(x)
#else
	#error("Are you a wizard?")
#endif



#endif