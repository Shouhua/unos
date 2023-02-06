/*
TODO: 
2022-02-06 依赖库解析，hello-dl依赖libmsg.so
1. 检索依赖项, 搜索依赖使用广度优先检索(breadth-first search)
DT_RUNPATH: $ORIGIN
DT_NEEDED: build/bin/libmsg.so
2. 解析relocation，广度优先的反向顺序, [libmsg, hello-dl]
主要是解析2类relocation：
DIRECT64: 
**注意检索symbol的时候使用广度优先的顺序，比如[hello-dl, libmsg], 这是msg先出现在hello-dl中，使用hello-dl的base addr**
hello-dl symbol table：msg 3000
hello-dl relocation：msg：offset 1007, symtable 1, addon 0
hello-dl base addr + 1007 = hello-dl base addr + 3000 + addon
COPY:
libmsg: symbol table: msg 2000 size 9
检索除去自己(hello-dl)之外的其他模块，找到msg，发现在libmsg的.dynsym中有
memcpy(*(hello-dl base addr + 1007), libmsg base addr + 2000, 9)
*/
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

#define RELA_UNIT 24

typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Dyn  Elf_Dyn;
typedef Elf64_Word Elf_Word;
typedef Elf64_Sxword Elf_Sxword;
typedef Elf64_Rela Elf_Rela;

Elf_Rela *first_rela;
Elf_Ehdr *file_header;

typedef struct node
{
	void *data;
	struct node *next;
} node_t;

// dynamic information
typedef struct dyn_info
{
	node_t *dyn_head;
	ulong start_addr;
	ulong total_size;
	ulong ent_size;
} dyn_info_t;

void node_push(node_t *head, void *data)
{
	if(head == NULL)
	{
		fprintf(stderr, "node push head == NULL");
		exit(EXIT_FAILURE);
	}
	node_t *current = head;
	while(current && current->next)
	{
		current = current->next;
	}
	current->next = (node_t *)malloc(sizeof(node_t));
	current->next->data = data;
	current->next->next = NULL;
}

node_t *phdr_head;
dyn_info_t *dyn_info;
node_t *rela_head;


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
	case 1:
		return "NEEDED";
		break;
	case 29:
		return "RUNPATH";
		break;
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
		case 1:
			return "DIRECT64";
			break;
		case 5:
			return "COPY";
			break;
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

static void parse_rela(Elf_Ehdr *header)
{
	for(int i=0; i< header->e_shnum; i++)
	{
		Elf_Shdr * sec_header = (Elf_Shdr*)((ulong)header+header->e_shoff+i*header->e_shentsize);
		// print relaction info
		// 只获取一个rela section
		if(sec_header->sh_type == SHT_RELA) {
			rela_head = (node_t *)malloc(sizeof(node_t));
			printf("Rela entries:\n");
			for(int j=0; j<(sec_header->sh_size/RELA_UNIT); j++)
			{
				Elf_Rela *rela = (Elf_Rela *)((ulong)header+sec_header->sh_offset+j*RELA_UNIT);
				printf("\toffset: 0x%08lx type: %8s sym: %ld addend: 0x%08lx\n",
					rela->r_offset,
					get_relo_type(rela->r_info&0xffffffff),
					rela->r_info>>32,
					rela->r_addend
				);

				node_push(rela_head, rela);
			}
			break;
		}
	}
}

static void parse_dyn(Elf_Ehdr *header, dyn_info_t *dyn_info) 
{
	dyn_info->dyn_head = (node_t *)malloc(sizeof(node_t));
	// 根据dynamic information填充dynamic tags
	for(int i=0; i<dyn_info->total_size/dyn_info->ent_size; i++)
	{
		Elf64_Dyn* dyn = (Elf64_Dyn*)((ulong)dyn_info->start_addr + dyn_info->ent_size * i);
		// if(dyn->d_tag == DT_RELA)
		// {
		// 	first_rela = (Elf_Rela*)((ulong)header+dyn->d_un.d_val);
		// }
		node_push(dyn_info->dyn_head, dyn);
	}
}

static void parse_phdr(Elf_Ehdr *header) 
{
	phdr_head = (node_t *)malloc(sizeof(node_t));
	Elf_Phdr *p_header= (Elf_Phdr *)((long)header + header->e_phoff);
	// node_push(phdr_head, p_header);
	for(int i = 0; i<header->e_phnum; i++)
	{
		Elf_Phdr *p_header = (Elf_Phdr *)malloc(sizeof(Elf_Phdr));
		memcpy(p_header, (Elf_Phdr *)((long)header + header->e_phoff + i * (header->e_phentsize)), sizeof(Elf_Phdr));
		// p_header= (Elf_Phdr *)((long)header + header->e_phoff + i * (header->e_phentsize));
		node_push(phdr_head, p_header);
	}
	// print program headers
	printf("Program Header:\n");
	node_t * current = phdr_head->next;
	while(current != NULL)
	{
		if(((Elf_Phdr*)current->data)->p_type == PT_DYNAMIC) 
		{
			dyn_info = (dyn_info_t *)malloc(sizeof(dyn_info_t));
			dyn_info->ent_size = 16;
			dyn_info->start_addr = (ulong)header+((Elf_Phdr*)current->data)->p_offset;
			dyn_info->total_size = ((Elf_Phdr*)current->data)->p_filesz;
			//parse dynamic tags
			parse_dyn(header, dyn_info);
		}
		printf("\tfile 0x%08lx..0x%08lx | mem 0x%08lx..0x%08lx | align 0x%08lx | %s%s%s %s\n",
			   ((Elf_Phdr*)(current->data))->p_offset,
			   ((Elf_Phdr*)current->data)->p_offset + ((Elf_Phdr*)current->data)->p_filesz,
			   ((Elf_Phdr*)current->data)->p_vaddr,
			   ((Elf_Phdr*)current->data)->p_vaddr + ((Elf_Phdr*)current->data)->p_memsz,
			   ((Elf_Phdr*)current->data)->p_align,
			   ((Elf_Phdr*)current->data)->p_flags & 0x0004 ? "R" : ".",
			   ((Elf_Phdr*)current->data)->p_flags & 0x0002 ? "W" : ".",
			   ((Elf_Phdr*)current->data)->p_flags & 0x0001 ? "X" : ".",
			   get_program_header_type(((Elf_Phdr*)current->data)->p_type));
		current = current->next;
	}
	if(dyn_info && dyn_info->dyn_head->next)
	{
		printf("Dynamic entries:\n");
		node_t *current_dyn = dyn_info->dyn_head->next;	
		while(current_dyn != NULL)
		{
			printf("\ttag: %-10s addr: 0x%08lx\n", get_dynamic_tag(((Elf_Dyn*)(current_dyn->data))->d_tag), ((Elf_Dyn*)(current_dyn->data))->d_un.d_val);
			current_dyn = current_dyn->next;
		}
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
	if(dyn) {
		printf("Dynamic entries:\n");
		while(dyn->d_tag)
		{
			if(dyn->d_tag == DT_RELA)
			{
				first_rela = (Elf_Rela*)((ulong)header+dyn->d_un.d_val);
			}
			printf("\ttag: %-10s addr: 0x%08lx\n", get_dynamic_tag(dyn->d_tag), dyn->d_un.d_val);
			dyn += 1;
		}
	}
}

static void _parse(Elf_Ehdr *header)
{
	_check_version(header);
	_print_type(header);
	_print_machine(header);
	printf("entry_point: 0x%lx\n", header->e_entry);
	// parse programe headers
	// 注意这个buf，暂时不能为file_header，尽管他们具有相同内容，但是函数里面需要使用buf的真实起始地址+offset，如果使用file_header，造成内存问题
	parse_phdr(header);
	parse_rela(header);
	// _print_program_header(header);
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
		exit(EXIT_FAILURE);
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

	// parse file header
	file_header = (Elf_Ehdr *)malloc(sizeof(Elf_Ehdr));
	memcpy(file_header, buf, sizeof(Elf_Ehdr));
	// file_header = (Elf_Ehdr *)buf;

	

	// 不要传递file_header, 因为file_header大小只有Elf_Ehdr, 没法根据它获取其他信息
	_parse((Elf_Ehdr *)buf);

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
		Elf_Phdr *p_header = (Elf_Phdr *)((long)buf + file_header->e_phoff + i * (file_header->e_phentsize));
		if (p_header->p_type == PT_LOAD && p_header->p_memsz > 0)
		{
			ulong start = base + p_header->p_vaddr;
			// TODO 是否需要使用p_header->p_align
			ulong aligned_start = start & ~0xFFF;
			ulong padding = start - aligned_start;
			ulong end = base + p_header->p_vaddr + p_header->p_memsz;
			ulong len = p_header->p_memsz + padding;
			printf("\tMapping segement @ 0x%08lx - 0x%08lx with 0x%x\n",
				   start, end, p_header->p_flags);
			printf("\tAddr: 0x%016lx, Padding: 0x%08lx\n", aligned_start, padding);
			if (mmap((void *)aligned_start, len, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0) == MAP_FAILED)
			{
				fprintf(stderr, "mmap failed: %d\n", errno);
				exit(EXIT_FAILURE);
			}
			printf("\tCopying segment data...\n");
			memcpy((void *)start, (void *)(buf + p_header->p_offset), p_header->p_memsz);
			printf("\tAdjusting permissions...\n");
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

			if(rela_head)
			{
				int num_relocs = 0;
				node_t * rela_node = rela_head->next;
				while(rela_node)
				{
					Elf_Rela *rela = rela_node->data;
					if(rela->r_offset>=p_header->p_offset && rela->r_offset<=p_header->p_offset+p_header->p_filesz) 
					{
						num_relocs += 1;
						// TODO 这里只是考虑了RELATIVE情况的改写
						*((int*)(base+rela->r_offset)) = base + rela->r_addend;
					}
					rela_node = rela_node->next;
				}
				if(num_relocs > 0)
				{
					printf("\tApplied %d relocations\n", num_relocs);
				}
			}
			mprotect((void *)aligned_start, len, p_prot);
			printf("\n");
		}
	}

	printf("Executing %s in memory...\n", argv[1]);
	printf("Jumping to entry point @ 0x%08lx\n", file_header->e_entry);

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

	free(file_header);
	file_header = NULL;
	free(phdr_head);
	phdr_head = NULL;

	close(exec_fd);
	return 0;
}
