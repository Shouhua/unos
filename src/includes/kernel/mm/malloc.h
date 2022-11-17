#ifndef __MALLOC_H__
#define __MALLOC_H__

#include "lib/stdint.h"

typedef struct {
	uint8_t status;
	uint32_t size;
} alloc_t;

void init_mm(uint32_t start, uint32_t end);

void* malloc(uint32_t size);
void free(void* mem);
void mm_print_info();

#endif