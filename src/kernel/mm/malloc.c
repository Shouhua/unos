#include "kernel/mm/malloc.h"
#include "lib/string.h"
#include "lib/log.h"

#define MAX_PAGE_ALIGNED_ALLOCS 32

uint32_t last_alloc = 0;
uint32_t heap_end = 0;
uint32_t heap_begin = 0;
uint32_t pheap_begin = 0;
uint32_t pheap_end = 0;
uint8_t *pheap_desc = 0;
uint32_t memory_used = 0;

void init_mm(uint32_t start, uint32_t end) {
	last_alloc = start;
	heap_begin = last_alloc;
	pheap_end = end; 
	pheap_begin = pheap_end - (MAX_PAGE_ALIGNED_ALLOCS * 4096);
	heap_end = pheap_begin;
	memset((uint8_t*)heap_begin, 0, heap_end - heap_begin);
	pheap_desc = (uint8_t*)malloc(MAX_PAGE_ALIGNED_ALLOCS);
	// printf("[MALLOC] Kernel heap start at 0x%x\n", last_alloc);
}

void mm_print_info() {
	printf("Memory used: %d bytes\n", memory_used);
	printf("Memory free: %d bytes\n", heap_end - heap_begin - memory_used);
	printf("Heap size: %d bytes\n", heap_end - heap_begin);
	printf("Heap start: 0x%x\n", heap_begin);
	printf("Heap end: 0x%x\n", heap_end);
	printf("PHeap start: 0x%x\nPHeap end: 0x%x\n", pheap_begin, pheap_end);
}

// TODO: 需要heap清理，因为根据malloc每次分配都会固定块的大小，当释放后块信息头还在那儿，需要全部看下，连续空块可以合并
void free(void* mem) {
	alloc_t* alloc = (mem - sizeof(alloc_t));
	memory_used -= alloc->size + sizeof(alloc_t);
	alloc->status = 0;
	printf("Free memeory: %x, size: %d\n", mem, alloc->size + sizeof(alloc_t));
}

void* malloc(uint32_t size) {
	if(!size) return 0;

		/* Loop through blocks and find a block sized the same or bigger */
		uint8_t *mem = (uint8_t *)heap_begin;
		while((uint32_t)mem < last_alloc)
		{
			alloc_t *a = (alloc_t *)mem;
			/* If the alloc has no size, we have reaced the end of allocation */
			//printf("mem=0x%x a={.status=%d, .size=%d}\n", mem, a->status, a->size);
			if(!a->size)
				goto nalloc;
			/* If the alloc has a status of 1 (allocated), then add its size
			* and the sizeof alloc_t to the memory and continue looking.
			*/
			if(a->status) {
				mem += a->size;
				mem += sizeof(alloc_t);
				mem += 4;
				continue;
			}
			/* If the is not allocated, and its size is bigger or equal to the
			* requested size, then adjust its size, set status and return the location.
			*/
			if(a->size >= size)
			{
				/* Set to allocated */
				a->status = 1;

				printf("RE:Allocated %d bytes from 0x%x to 0x%x\n", size, mem + sizeof(alloc_t), mem + sizeof(alloc_t) + size);
				memset(mem + sizeof(alloc_t), 0, size);
				memory_used += size + sizeof(alloc_t);
				return (void *)(mem + sizeof(alloc_t));
			}
			/* If it isn't allocated, but the size is not good, then
			* add its size and the sizeof alloc_t to the pointer and
			* continue;
			*/
			mem += a->size;
			mem += sizeof(alloc_t);
			mem += 4;
		}

		nalloc:;
		if(last_alloc+size+sizeof(alloc_t) >= heap_end)
		{
			// set_task(0);
			PANIC("[Malloc] Cannot allocate bytes! Out of memory.\n");
		}
		alloc_t *alloc = (alloc_t *)last_alloc;
		alloc->status = 1;
		alloc->size = size;

		last_alloc += size;
		last_alloc += sizeof(alloc_t);
		last_alloc += 4;
		// printf("Allocated %d bytes from 0x%x to 0x%x\n", size, (uint32_t)alloc + sizeof(alloc_t), last_alloc);
		memory_used += size + 4 + sizeof(alloc_t);
		memset((char *)((uint32_t)alloc + sizeof(alloc_t)), 0, size);
		return (void *)((uint32_t)alloc + sizeof(alloc_t));
	/*
		char* ret = (char*)last_alloc;
		last_alloc += size;
		if(last_alloc >= heap_end)
		{
			set_task(0);
			panic("Cannot allocate %d bytes! Out of memory.\n", size);
		}
		printf("Allocated %d bytes from 0x%x to 0x%x\n", size, ret, last_alloc);
		return ret;*/
}

/*
	malloc a s block and memset
*/
void* kcalloc(uint32_t num, uint32_t size) {
    void * ptr = malloc(num * size);
    memset(ptr, 0, num*size);
    return ptr;
}
/*
wrapper function for free
*/
void kfree(void * ptr) {
    free(ptr);
}