#ifndef UNOS_TYPES_H
#define UNOS_TYPES_H

#define EOF -1 // end of file
#define NULL ((void *)0) // null pointer

#define EOS '\0'
#define CONCAT(x, y) x##y
#define RESERVED_TOKEN(x, y) CONCAT(x, y)
#define RESERVED RESERVED_TOKEN(reserved, __LINE__)

#ifndef __cplusplus
#define bool _Bool
#define true 1
#define false 0
#endif

// 用于定义特殊的结构体
#define _packed __attribute__((packed))

// 用于省略函数的栈帧
#define _ofp __attribute__((optimize("omit-frame-pointer")))

#define _inline __attribute__((always_inline)) inline

typedef unsigned int size_t;

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef int32 pid_t;
typedef int32 dev_t;

typedef u32 time_t;
typedef u32 idx_t;

typedef int32 fd_t;
typedef enum std_fd_t
{
    stdin,
    stdout,
    stderr,
} std_fd_t;

#endif