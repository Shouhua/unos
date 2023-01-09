/*
    gcc dump_stack.c -g -o dump_stack
    gdb dump_stack
    b main
    r
    argc/argv都没有在symbol出现，因为他们存在于寄存器或者堆栈中，argc在edi中，argv在$esp中
*/
#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <string.h>

int main(int argc, char** argv) {
    int i = 0;
    Elf64_auxv_t *auxv = NULL;
    long *auxv_start_addr = NULL;
    long *auxv_end_addr = NULL;
    long *env_start_addr = NULL;
    long *env_end_addr = NULL;
    long *argv_end_addr = NULL;
    char *platform;
    long *p = (long *)argv;
    char *env_str = NULL;
    unsigned char *rand_bytes = NULL;
    int arg_num = *(p - 1);
    printf("arg_num:%d, argc addr: 0x%x\n", arg_num, p-1);
    for (i = 0;i < arg_num;i++) {
        printf("arg %d:%s, at: 0x%x\n", i, *p, p); 
        p++;
    }
    argv_end_addr = p;
    p++;//skip null
    printf("\nenvironment vector:\n");
    env_str = (char *)*p;
    env_start_addr = p;
    while (*p) {
        printf("%s, at: 0x%x\n", *p, p); 
        p++;
    }
    env_end_addr = p;
    printf("env end addr: 0x%x\n", p);
    p++;//skip null
    printf("\nauxiliary vectors:\n");
    auxv = (Elf64_auxv_t *)p; 
    auxv_start_addr = (long *)auxv;
    while (auxv->a_type != AT_NULL) {
        printf("auxv type:%ld, val:0x%x\n", auxv->a_type, auxv->a_un.a_val); 
        if(auxv->a_type == AT_EXECFN) {
            printf("AT_EXECFN content is: %s\n", auxv->a_un.a_val);
        }
        if (auxv->a_type == AT_PLATFORM) {
            platform = (char *)(auxv->a_un.a_val);
        }
        auxv++;
    }
    auxv_end_addr = (long *)auxv;
    auxv++;//skip AT_NULL vector
    printf("\naddress:%lx, env0_ascii_string:%s\n", env_str, env_str);
    printf("address:%lx ~ %lx, argument0_ascii_string:%s\n", *argv, argv[0] + strlen(argv[0]),*argv);
    printf("address:%lx ~ %lx, here contain %ld bytes data\n", platform + strlen(platform) + 1, (char *)*argv - 1, (long)*argv - (long)platform - strlen(platform) - 1);
    printf("address:%lx ~ %lx, String identifying platform:%s\n", platform, platform + strlen(platform), platform);
    rand_bytes = platform - 16; 
    printf("address:%lx ~ %lx, rand_bytes[16]:", rand_bytes, rand_bytes + 15);
    for (i = 0;i < 16;i++) {
        printf(" %d", *(rand_bytes + i)); 
    }
    printf("\naddress:%lx ~ %lx, auxivilary vector\n", auxv_start_addr, auxv_end_addr);
    printf("address:%lx ~ %lx, environment vector\n", env_start_addr, env_end_addr);
    printf("address:%lx ~ %lx, argv, argv pointer: %lx\n", argv, argv_end_addr, *argv);
    printf("address:%lx, argc\n", argv - 1);
    return 0;
}
