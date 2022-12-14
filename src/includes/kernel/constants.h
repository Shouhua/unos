#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#define K 1024
#define M (1024 * K)
#define G (1024 * M)

#define K_STACK_SIZE            0x100000U
#define K_STACK_START           ((0xFFBFFFFFU - K_STACK_SIZE) + 1)
#define HIGHER_HLF_BASE         0xC0000000UL
#define MEM_1MB                 0x100000UL

#define VGA_BUFFER_VADDR        0xB0000000UL
#define VGA_BUFFER_PADDR        0xB8000UL
#define VGA_BUFFER_SIZE         4096

#endif
