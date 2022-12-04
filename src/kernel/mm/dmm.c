#include "kernel/mm/malloc.h"
#include "lib/stdint.h"
#include "lib/log.h"
#include "lib/string.h"

static char* mem_heap;
static char* mem_brk;
static char* mem_max_adrr;

// dmm_init(_kernel_end, 4M-64k-_kernel_end)
void
dmm_init(void* start, uint32_t size)
{
    mem_heap = (char*)start;
    mem_brk = (char*)mem_heap;
    mem_max_adrr = (char*)(mem_heap + size);
}

void*
dmm_sbrk(int incr)
{
    char* old_brk = mem_brk;
    if((incr < 0) || ((mem_brk + incr) > mem_max_adrr)) {
        PANIC("mem_sbrk failed. Ran out of memory...\n");
        return (void*)-1;
    }
    mem_brk += incr;
    return (void*)old_brk;
}
