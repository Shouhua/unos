#ifndef __KMALLOC_H__
#define __KMALLOC_H__

#include "lib/stdint.h"

#define WSIZE       4
#define DSIZE       8
#define CHUNKSIZE   (1<<12) // 默认扩展4k大小
#define OVERHEAD    8       /* overhead of header and footer (bytes) */

#define MAX(x,y)    ((x)>(y)) ? (x) : (y)

#define PACK(size, alloc)   ((size) | (alloc))

#define GET(p)  (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (val))

#define GET_SIZE(p)  (GET(p) & ~0x07)
#define GET_ALLOC(p)  (GET(p) & ~0x1)

#define HDRP(bp)    ((char*)(bp) - WSIZE)
#define FTRP(bp)    ((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp)   ((char*)(bp) + GET_SIZE(((char*)(bp) - WSIZE)))
#define PREV_BLKP(bp)   ((char*)(bp) - GET_SIZE(((char*)(bp) - DSIZE)))

int mm_init(void);
void* mm_malloc(size_t size);
void mm_free(void* ptr);

#endif