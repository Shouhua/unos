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

static void _check_version(Elf_Ehdr *header) {
    assert(header->e_ident[EI_MAG0] == ELFMAG0);
    assert(header->e_ident[EI_MAG1] == ELFMAG1);
    assert(header->e_ident[EI_MAG2] == ELFMAG2);
    assert(header->e_ident[EI_MAG3] == ELFMAG3);
    printf("input is a supported ELF file!\n");
}
static void _print_type(Elf_Ehdr *header) {
    switch (header->e_type) {
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
static void _print_machine(Elf_Ehdr *header) {
    switch (header->e_machine) {
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
static void _print_program_header(Elf_Ehdr *header) {
    printf("prgram_headers:\n");
    for(int i=0; i<header->e_phnum; i++) {
        Elf_Phdr *p_header = (Elf_Phdr *)((long)header+header->e_phoff+i*(header->e_phentsize));
        printf("\tfile 0x%08x..0x%08x | mem 0x%08x..0x%08x | align 0x%08x | %s%s%s %s\n", 
            p_header->p_offset, 
            p_header->p_offset+p_header->p_filesz,
            p_header->p_vaddr, 
            p_header->p_vaddr+p_header->p_memsz,
            p_header->p_align,
            p_header->p_flags & 0x0004 ? "R" : ".",
            p_header->p_flags & 0x0002 ? "W" : ".",
            p_header->p_flags & 0x0001 ? "X" : ".",
            p_header->p_type & 0x0001 ? "LOAD" : "OTHER"
        );

    }
}

static void _parse(Elf_Ehdr *header) {
    _check_version(header);
    _print_type(header);
    _print_machine(header);
    printf("entry_point: 0x%x\n", header->e_entry);
    _print_program_header(header);
}

// return elf program return value
static int _exec_elf(char *path, char *const argv[]) {
    pid_t p;
    int stat;
    int child_pid;
    
    p = fork();
    if(p == 0) {
        execv(path, argv);
    }
    else
    {
        child_pid = wait(&stat);
        if(WIFEXITED(stat))
            return WEXITSTATUS(stat);
        else
            return -127;
    }
}

static void _disassemble_hello(Elf_Ehdr *file_header, char *buf) {

    int skip = 0x1000;
    int size = 0x25;
    char *asm_code = (char *)malloc(size);
    // strncpy遇到0停止copy
    // strncpy(asm_code, buf+skip, size);
    memcpy(asm_code, buf+skip, size);

    int err;
    int fildes[2];
    if(pipe(fildes) == -1) {
        fprintf(stderr, "create pipe fildes failed: %d\n", errno);
        exit(-1);
    }

    pid_t pid;
    int status;
    char disassembly_code[1024];
    pid = fork();
    if(pid == 0) { // child process
        if(dup2(fildes[0], STDIN_FILENO) == -1) {
            exit(errno);
        }
        close(fildes[0]);
        close(fildes[1]);

        int result = execl("/usr/bin/ndisasm", "ndisasm", "-b", "64", "-", NULL);
        exit(result);
    }
    else {
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

int main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(stderr, "usage: elk FILE\n");
        exit(-1);
    }

    int exec_fd = open(argv[1], O_RDONLY, 0);
    if(exec_fd == -1) {
        fprintf(stderr, "open exec file error\n");
        exit(errno);
    }

    printf("Analyzing \"%s\"...\n", argv[1]);

    // 获取文件大小，读取到内存
    int size = lseek(exec_fd, 0, SEEK_END);
    lseek(exec_fd, 0, SEEK_SET);
    char * buf = malloc(size);
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
    Elf_Phdr *entry_p_header = NULL;
    for(int i=0; i<file_header->e_phnum; i++) {
        Elf_Phdr *p_header = (Elf_Phdr *)((long)file_header+file_header->e_phoff+i*(file_header->e_phentsize));
        if(file_header->e_entry >= p_header->p_vaddr && file_header->e_entry <= (p_header->p_vaddr + p_header->p_memsz)) 
        {
            entry_p_header = p_header;
            break;
        }
    }
    if(!entry_p_header) {
        fprintf(stderr, "Get entry program header failed\n");
        exit(EXIT_FAILURE);
    }

    // mprotect必须使用4k对齐地址，不然出现errno=22
    
    if(mprotect((void*)(((long)buf+entry_p_header->p_offset) & (~0xFFF)), entry_p_header->p_memsz, PROT_READ|PROT_WRITE|PROT_EXEC) == -1) {
        fprintf(stderr, "mprotect failed: error %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("Executing %s in memory...\n", argv[1]);
    int (*ptr)();
    // ptr = (int (*)())file_header->e_entry;
    ptr = (int (*)())(buf+entry_p_header->p_offset);
    ptr(argc, argv, NULL);

    free(buf);
    buf = NULL;
    
    return 0;
}
