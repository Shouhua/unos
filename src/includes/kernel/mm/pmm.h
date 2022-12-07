#ifndef __PMM_H__
#define __PMM_H__

#include <lib/stdint.h>

#define PMM_BLOCKS_PER_BYTE 	8
#define PMM_BLOCK_SIZE 			4096
#define PMM_BLOCK_ALIGN 		PMM_BLOCK_SIZE
#define PMM_BITMAP_SIZE			32768 // 4G / 4096 / 4*8 4G最多使用128k大小数组

typedef uint32_t physical_addr;

void init_pmm(size_t);

//! enables a physical memory region for use
void	pmm_init_region (physical_addr, size_t);

//! disables a physical memory region as in use (unuseable)
void	pmm_deinit_region (physical_addr base, size_t);

//! allocates a single memory block
void*	pmm_alloc_block ();

//! releases a memory block
void	pmm_free_block (void*);

//! allocates blocks of memory
void*	pmm_alloc_blocks (size_t);

//! frees blocks of memory
void	pmm_free_blocks (void*, size_t);

//! returns amount of physical memory the manager is set to use
size_t pmm_get_memory_size ();

//! returns number of blocks currently in use
uint32_t pmm_get_used_block_count ();

//! returns number of blocks not in use
uint32_t pmm_get_free_block_count ();

//! returns number of memory blocks
uint32_t pmm_get_block_count ();

//! returns default memory block size in bytes
uint32_t pmm_get_block_size ();

//! enable or disable paging
void pmm_paging_enable (bool);

//! test if paging is enabled
bool pmm_is_paging ();

//! loads the page directory base register (PDBR)
void pmm_load_PDBR (physical_addr);

//! get PDBR physical address
physical_addr pmm_get_PDBR ();

#endif