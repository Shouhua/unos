#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include <elf.h>

typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Dyn  Elf_Dyn;
typedef Elf64_Word Elf_Word;
typedef Elf64_Sxword Elf_Sxword;
typedef Elf64_Rela Elf_Rela;

static void _check_version(Elf_Ehdr *header)
{
	assert(header->e_ident[EI_MAG0] == ELFMAG0);
	assert(header->e_ident[EI_MAG1] == ELFMAG1);
	assert(header->e_ident[EI_MAG2] == ELFMAG2);
	assert(header->e_ident[EI_MAG3] == ELFMAG3);
	printf("input is a supported ELF file!\n");
}
static void _print_type(Elf_Ehdr *header)
{
	switch (header->e_type)
	{
	case ET_REL:
		printf("type: %s\n", "REL");
		break;
	case ET_EXEC:
		printf("type: %s\n", "EXEC");
		break;
	case ET_DYN:
		printf("type: %s\n", "DYN");
		break;
	default:
		printf("unsupported elf file type");
	}
}
static void _print_machine(Elf_Ehdr *header)
{
	switch (header->e_machine)
	{
	case EM_X86_64:
		printf("machine: %s\n", "X86_64");
		break;
	case EM_386:
		printf("machine: %s\n", "X86");
		break;
	default:
		printf("unsupported elf file machine");
	}
}

static inline char * get_program_header_type(Elf64_Word type)
{
	switch (type)
	{
	case 0:
		return "NULL";
		break;
	case 1:
		return "LOAD";
		break;
	case 2:
		return "DYNAMIC";
		break;
	case 3:
		return "INTERP";
		break;
	case 6:
		return "PHDR";
		break;
	case 0x6474e552:
		return "PHDR";
		break;
	default:
		return "OTHER";
		break;
	}
}
static inline char * get_dynamic_tag(Elf_Sxword tag)
{
	switch (tag)
	{
	case 4:
		return "HASH";
		break;
	case 5:
		return "STRTAB";
		break;
	case 6:
		return "SYMTAB";
		break;
	case 7:
		return "RELA";
		break;
	case 8:
		return "RELASZ";
		break;
	case 9:
		return "RELAENT";
		break;
	case 10:
		return "STRSZ";
		break;
	case 11:
		return "SYMENT";
		break;
	case 0x15:
		return "DEBUG";
		break;
	case 0x16:
		return "TEXTREL";
		break;
	case 0x1e:
		return "FLAGS";
		break;
	case 0x6ffffef5:
		return "GNU_HASH";
		break;
	case 0x6ffffff9:
		return "RELACOUNT";
		break;
	case 0x6ffffffb:
		return "FLAGS_1";
		break;
	default:
		return "OTHER";
		break;
	}
}
static inline char *get_relo_type(ulong type) 
{
	switch(type)
	{
		case 6:
			return "GlobDat";
			break;
		case 7:
			return "JumpSlot";
			break;
		case 8:
			return "Relative";
			break;
		default:
			return "Other";
			break;
	}
}

static void _print_program_header(Elf_Ehdr *header)
{
	printf("prgram_headers:\n");
	Elf_Dyn *dyn;
	ulong dyn_size;
	for (int i = 0; i < header->e_phnum; i++)
	{
		Elf_Phdr *p_header = (Elf_Phdr *)((long)header + header->e_phoff + i * (header->e_phentsize));
		if(p_header->p_type == PT_DYNAMIC) {
			dyn = (Elf_Dyn *)(((ulong)header)+p_header->p_offset);
			dyn_size = p_header->p_filesz;
		}
		printf("\tfile 0x%08lx..0x%08lx | mem 0x%08lx..0x%08lx | align 0x%08lx | %s%s%s %s\n",
			   p_header->p_offset,
			   p_header->p_offset + p_header->p_filesz,
			   p_header->p_vaddr,
			   p_header->p_vaddr + p_header->p_memsz,
			   p_header->p_align,
			   p_header->p_flags & 0x0004 ? "R" : ".",
			   p_header->p_flags & 0x0002 ? "W" : ".",
			   p_header->p_flags & 0x0001 ? "X" : ".",
			   get_program_header_type(p_header->p_type));
	}
	Elf_Rela *rela;
	if(dyn) {
		printf("Dynamic entries:\n");
		while(dyn->d_tag)
		{
			if(dyn->d_tag == DT_RELA)
			{
				rela = (Elf_Rela*)((ulong)header+dyn->d_un.d_val);
			}
			printf("\ttag: %-10s addr: 0x%08lx\n", get_dynamic_tag(dyn->d_tag), dyn->d_un.d_val);
			dyn += 1;
		}
	}

	if(rela) 
	{
		ulong rela_unit = 24;

		// TODO get section info
		for(int i=0; i< header->e_shnum; i++)
		{
			Elf_Shdr * sec_header = (Elf_Shdr*)((ulong)header+header->e_shoff+i*header->e_shentsize);
			// print relaction info
			if(sec_header->sh_type == SHT_RELA) {
				printf("Rela entries:\n");
				for(int j=0; j<(sec_header->sh_size/rela_unit); j++)
				{
					printf("\toffset: 0x%08lx type: %8s sym: %ld addend: 0x%08lx\n",
						rela->r_offset,
						get_relo_type(rela->r_info&0xffffffff),
						rela->r_info>>32,
						rela->r_addend
					);
				}
			}
		}
	}
}

static void _parse(Elf_Ehdr *header)
{
	_check_version(header);
	_print_type(header);
	_print_machine(header);
	printf("entry_point: 0x%lx\n", header->e_entry);
	_print_program_header(header);
}

// return elf program return value
static int _exec_elf(char *path, char *const argv[])
{
	pid_t p;
	int stat;
	int child_pid;

	p = fork();
	if (p == 0)
	{
		execv(path, argv);
	}
	else
	{
		child_pid = wait(&stat);
		if (WIFEXITED(stat))
			return WEXITSTATUS(stat);
		else
			return -127;
	}
}

static void _disassemble_hello(Elf_Ehdr *file_header, char *buf)
{

	int skip = 0x1000;
	int size = 0x25;
	char *asm_code = (char *)malloc(size);
	// strncpy遇到0停止copy
	// strncpy(asm_code, buf+skip, size);
	memcpy(asm_code, buf + skip, size);

	int err;
	int fildes[2];
	if (pipe(fildes) == -1)
	{
		fprintf(stderr, "create pipe fildes failed: %d\n", errno);
		exit(-1);
	}

	pid_t pid;
	int status;
	char disassembly_code[1024];
	pid = fork();
	if (pid == 0)
	{ // child process
		if (dup2(fildes[0], STDIN_FILENO) == -1)
		{
			exit(errno);
		}
		close(fildes[0]);
		close(fildes[1]);

		int result = execl("/usr/bin/ndisasm", "ndisasm", "-b", "64", "-", NULL);
		exit(result);
	}
	else
	{
		close(fildes[0]);
		write(fildes[1], asm_code, size);
		close(fildes[1]);

		printf("%s\n", disassembly_code);
		waitpid(-1, &status, 0);
		// printf("Parent: child created with pid = %d, status=%d\n", pid, status);
	}
	free(asm_code);
	asm_code = NULL;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "usage: elk FILE\n");
		exit(-1);
	}

	int exec_fd = open(argv[1], O_RDONLY, 0);
	if (exec_fd == -1)
	{
		fprintf(stderr, "open exec file error\n");
		exit(errno);
	}

	printf("Analyzing \"%s\"...\n", argv[1]);

	// 获取文件大小，读取到内存
	int size = lseek(exec_fd, 0, SEEK_END);
	lseek(exec_fd, 0, SEEK_SET);
	char *buf = malloc(size);
	read(exec_fd, buf, size);

	Elf_Ehdr *file_header = (Elf_Ehdr *)buf;

	// 解析elf文件信息
	_parse(file_header);

	// 使用execve执行elf文件
	// printf("\nExecuting %s\n", argv[1]);
	// _exec_elf(argv[1], NULL);

	// // disassemble elf文件程序段
	// printf("\nDisassembly \"%s\"\n", argv[1]);
	// _disassemble_hello(file_header, buf);

	// 在内存中执行elf文件
	// Elf_Phdr *entry_p_header = NULL;
	// for(int i=0; i<file_header->e_phnum; i++) {
	//     Elf_Phdr *p_header = (Elf_Phdr *)((long)file_header+file_header->e_phoff+i*(file_header->e_phentsize));
	//     if(file_header->e_entry >= p_header->p_vaddr && file_header->e_entry <= (p_header->p_vaddr + p_header->p_memsz))
	//     {
	//         entry_p_header = p_header;
	//         break;
	//     }
	// }
	// if(!entry_p_header) {
	//     fprintf(stderr, "Get entry program header failed\n");
	//     exit(EXIT_FAILURE);
	// }

	// mprotect必须使用4k对齐地址，不然出现errno=22

	// if(mprotect((void*)(((long)buf+entry_p_header->p_offset) & (~0xFFF)), entry_p_header->p_memsz, PROT_READ|PROT_WRITE|PROT_EXEC) == -1) {
	//     fprintf(stderr, "mprotect failed: error %d\n", errno);
	//     exit(EXIT_FAILURE);
	// }

	printf("Mapping %s in memory\n", argv[1]);
	unsigned int base = 0x400000;
	for (int i = 0; i < file_header->e_phnum; i++)
	{
		Elf_Phdr *p_header = (Elf_Phdr *)((long)file_header + file_header->e_phoff + i * (file_header->e_phentsize));
		if (p_header->p_type == PT_LOAD && p_header->p_memsz > 0)
		{
			ulong start = base + p_header->p_vaddr;
			ulong aligned_start = start & ~0xFFF;
			ulong padding = start - aligned_start;
			ulong end = base + p_header->p_vaddr + p_header->p_memsz;
			ulong len = p_header->p_memsz + padding;
			printf("Mapping segement @ 0x%08lx - 0x%08lx with 0x%x\n",
				   start, end, p_header->p_flags);
			printf("Addr: 0x%016lx, Padding: 0x%08lx\n", aligned_start, padding);
			if (mmap((void *)aligned_start, len, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0) == MAP_FAILED)
			{
				fprintf(stderr, "mmap failed: %d\n", errno);
				exit(EXIT_FAILURE);
			}
			printf("Copying segment data...\n");
			memcpy((void *)start, (void *)(buf + p_header->p_offset), p_header->p_memsz);
			printf("Adjusting permissions...\n");
			int p_prot = PROT_NONE;
			if (p_header->p_flags & 0x0004)
			{
				p_prot |= PROT_READ;
			}
			if (p_header->p_flags & 0x0002)
			{
				p_prot |= PROT_WRITE;
			}
			if (p_header->p_flags & 0x0001)
			{
				p_prot |= PROT_EXEC;
			}
			mprotect((void *)aligned_start, len, p_prot);
		}
	}

	printf("Executing %s in memory...\n", argv[1]);
	printf("Jumping to entry point: 0x%016lx\n", file_header->e_entry);

	int (*ptr)();
	ptr = (int (*)())(base+file_header->e_entry);
	// ptr = (int (*)())(buf+entry_p_header->p_offset);

	// printf("      code   @ 0x%016lx\n", buf+entry_p_header->p_vaddr);
	// printf("entry offset @ 0x%016lx\n", file_header->e_entry - entry_p_header->p_vaddr);
	// printf("entry point  @ 0x%016lx\n", buf+file_header->e_entry);

	printf("Press enter to continue...");
	while (getchar() != '\n')
		;
	ptr(argc, argv, NULL);

	free(buf);
	buf = NULL;

	return 0;
}
