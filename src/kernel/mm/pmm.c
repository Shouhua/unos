#include "kernel/mm/pmm.h"
#include "lib/string.h"

static uint32_t _pmm_memory_size = 0;
static uint32_t _pmm_used_blocks = 0;
static uint32_t _pmm_max_blocks = 0;
static uint32_t *_pmm_memory_map = 0;

static uint32_t bitmap_addr[PMM_BITMAP_SIZE];

// 标记内存已占用
inline static void 
mmap_set(uint32_t bit)
{
	_pmm_memory_map[bit / 32] |= (1 << (bit % 32));
}

// 标记内存可用
inline static void
mmap_unset(uint32_t bit)
{
	_pmm_memory_map[bit / 32] &= ~(1 << (bit % 32));
}

// 测试是否不可用，返回1表示不可用, 0表示可用
inline static bool
mmap_test(int bit)
{
	return _pmm_memory_map[bit / 32] & (1 << (bit % 32));
}

// 总共分配的内存大小
size_t
pmm_get_memory_size ()
{
	return _pmm_memory_size;
}

// 默认分配内存大小4k
uint32_t
pmm_get_block_size ()
{
	return PMM_BLOCK_SIZE;
}

// 剩下内存block
uint32_t
pmm_get_free_block_count ()
{
	return _pmm_max_blocks - _pmm_used_blocks;
}

// 已使用blocks
uint32_t
pmm_get_used_block_count ()
{
	return _pmm_used_blocks;
}

// 最大多少block
uint32_t
pmm_get_block_count()
{
	return _pmm_max_blocks;
}

// returns index of first free bit in bit map
int
mmap_first_free()
{
	for (uint32_t i = 0; i < pmm_get_block_count() / 32; i++)
		if (_pmm_memory_map[i] != 0xffffffff)
			for (int j = 0; j < 32; j++)
			{ //! test each bit in the dword
				int bit = 1 << j;
				if (!(_pmm_memory_map[i] & bit)) // 为0表示可使用
					return i * 4 * 8 + j;
			}
	return -1;
}

//! finds first free "size" number of frames and returns its index
int
mmap_first_free_s (size_t size)
{

	if (size == 0)
		return -1;

	if (size == 1)
		return mmap_first_free ();

	for (uint32_t i = 0; i < pmm_get_block_count(); i++)
		if (_pmm_memory_map[i] != 0xffffffff)
			for (int j=0; j<32; j++) {	//! test each bit in the dword

				int bit = 1<<j;
				if (! (_pmm_memory_map[i] & bit) ) {

					int startingBit = i*32;
					startingBit += bit;		//get the free bit in the dword at index i

					uint32_t free = 0; //loop through each bit to see if its enough space
					for (uint32_t count = 0; count <= size; count++) {
						if (! mmap_test (startingBit+count) )
							free++;	// this bit is clear (free frame)

						if (free == size)
							return i*4*8+j; //free count==size needed; return index
					}
				}
			}
	return -1;
}

void
init_pmm(size_t mem_size)
{
	_pmm_memory_size = mem_size;
	_pmm_memory_map = bitmap_addr;
	_pmm_max_blocks = pmm_get_memory_size() / PMM_BLOCK_SIZE;
	_pmm_used_blocks = pmm_get_block_count();
	// 全部标记为不可用，后面根据multiboot信息标记可用区域
	memset(_pmm_memory_map, 0xff, pmm_get_block_count() / PMM_BLOCKS_PER_BYTE);
}

// 标记内存区域可用
void
pmm_init_region(physical_addr base, size_t size)
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

void
pmm_deinit_region(physical_addr base, size_t size)
{
	int align = base / PMM_BLOCK_SIZE;
	int blocks = size / PMM_BLOCK_SIZE;

	for (; blocks > 0; blocks--)
	{
		mmap_set(align++);
		_pmm_used_blocks++;
	}
}

void*
pmm_alloc_block()
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

void
pmm_free_block(void *p)
{
	physical_addr addr = (physical_addr)p;
	int frame = addr / PMM_BLOCK_SIZE;

	mmap_unset(frame);

	_pmm_used_blocks--;
}

void*
pmm_alloc_blocks (size_t size)
{
	if (pmm_get_free_block_count() <= size)
		return 0;	//not enough space

	int frame = mmap_first_free_s (size);

	if (frame == -1)
		return 0;	//not enough space

	for (uint32_t i=0; i<size; i++)
		mmap_set (frame+i);

	physical_addr addr = frame * PMM_BLOCK_SIZE;
	_pmm_used_blocks+=size;

	return (void*)addr;
}

void
pmm_free_blocks (void* p, size_t size)
{
	physical_addr addr = (physical_addr)p;
	int frame = addr / PMM_BLOCK_SIZE;

	for (uint32_t i=0; i<size; i++)
		mmap_unset (frame+i);

	_pmm_used_blocks-=size;
}