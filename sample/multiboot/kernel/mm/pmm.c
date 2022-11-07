#include "kernel/mm/pmm.h"
#include "lib/string.h"

#define PMM_BLOCKS_PER_BYTE 8
#define PMM_BLOCK_SIZE 4096
#define PMM_BLOCK_ALIGN PMM_BLOCK_SIZE
#define PMM_BITMAP_MAX_SIZE (128 * 1024) // 1G need 1G/1024/8 = 128K bitmap

uint32_t bitmap[PMM_BITMAP_MAX_SIZE];
static uint32_t _pmm_memory_size = 0;
static uint32_t _pmm_used_blocks = 0;
static uint32_t _pmm_max_blocks = 0;
static uint32_t *_pmm_memory_map = 0;

// uint32_4 4 bytes 4*8bits
inline static void mmap_set(uint32_t bit)
{
	_pmm_memory_map[bit / 32] |= (1 << (bit % 32));
}

inline static void mmap_unset(uint32_t bit)
{
	_pmm_memory_map[bit / 32] &= ~(1 << (bit % 32));
}

inline static bool mmap_test(int bit)
{
	return _pmm_memory_map[bit / 32] & (1 << (bit % 32));
}

size_t	pmm_get_memory_size () {

	return _pmm_memory_size;
}

uint32_t pmm_get_block_size () {

	return PMM_BLOCK_SIZE;
}

uint32_t pmm_get_free_block_count () {

	return _pmm_max_blocks - _pmm_used_blocks;
}

uint32_t pmm_get_used_block_count () {

	return _pmm_used_blocks;
}

uint32_t pmm_get_block_count()
{
	return _pmm_max_blocks;
}

// returns index of first free bit in bit map
int mmap_first_free()
{
	for (uint32_t i = 0; i < pmm_get_block_count() / 32; i++)
		if (_pmm_memory_map[i] != 0xffffffff)
			for (int j = 0; j < 32; j++)
			{ //! test each bit in the dword
				int bit = 1 << j;
				if (!(_pmm_memory_map[i] & bit))
					return i * 4 * 8 + j;
			}
	return -1;
}

void init_pmm(size_t mem_size)
{
	_pmm_memory_size = mem_size;
	_pmm_memory_map = (uint32_t *)bitmap;
	_pmm_max_blocks = (pmm_get_memory_size() * 1024) / PMM_BLOCK_SIZE;
	_pmm_used_blocks = pmm_get_block_count();
	memset(_pmm_memory_map, 0xf, pmm_get_block_count() / PMM_BLOCKS_PER_BYTE);
}

void pmm_init_region(physical_addr base, size_t size)
{
	int align = base / PMM_BLOCK_SIZE;
	int blocks = size / PMM_BLOCK_SIZE;

	for (; blocks > 0; blocks--)
	{
		mmap_unset(align++);
		_pmm_used_blocks--;
	}

	mmap_set(0); // first block is always set. This insures allocs cant be 0
}

void pmm_deinit_region(physical_addr base, size_t size)
{
	int align = base / PMM_BLOCK_SIZE;
	int blocks = size / PMM_BLOCK_SIZE;

	for (; blocks > 0; blocks--)
	{
		mmap_set(align++);
		_pmm_used_blocks++;
	}
}

void *pmm_alloc_block()
{
	if (pmm_get_free_block_count() <= 0)
		return 0; // out of memory

	int frame = mmap_first_free();

	if (frame == -1)
		return 0; // out of memory

	mmap_set(frame);

	physical_addr addr = frame * PMM_BLOCK_SIZE;
	_pmm_used_blocks++;

	return (void *)addr;
}

void pmm_free_block(void *p)
{
	physical_addr addr = (physical_addr)p;
	int frame = addr / PMM_BLOCK_SIZE;

	mmap_unset(frame);

	_pmm_used_blocks--;
}