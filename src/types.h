#ifndef UNOS_TYPES_H
#define UNOS_TYPES_H

#define EOF -1 // end of file
#define NULL ((void *)0) // null pointer

#define EOS '\0'
#define CONCAT(x, y) x##y
#define RESERVED_TOKEN(x, y) CONCAT(x, y)
#define RESERVED RESERVED_TOKEN(reserved, __LINE__)

// 参考标准库stdbool.h
#ifndef __cplusplus
/*
 * Only defined for C, since C++ has its own bool support
 * */
/** Expands to the integer constant 1. */
#define true 1
/** Expands to the integer constant 0. */
#define false 0
/** Expands to _Bool. */
typedef _Bool bool;
#endif

// 用于定义特殊的结构体
#define _packed __attribute__((packed))

// 用于省略函数的栈帧
#define _ofp __attribute__((optimize("omit-frame-pointer")))
#define _inline __attribute__((always_inline)) inline

// 使用32bits方式定义，类型参考标准库stdint.h
typedef unsigned long size_t;
typedef long ssize_t;

typedef char int8_t; // default signed char
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef int32_t pid_t;
typedef int32_t dev_t;

typedef uint32_t time_t;
typedef uint32_t idx_t;

typedef int32_t fd_t;
typedef enum std_fd_t
{
    stdin,
    stdout,
    stderr,
} std_fd_t;

#endif