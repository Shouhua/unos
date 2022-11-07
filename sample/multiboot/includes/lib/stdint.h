#ifndef __STDINT_H__
#define __STDINT_H__

#define _packed __attribute__((packed))

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

/** Expands to the integer constant 1. */
#define true 1
/** Expands to the integer constant 0. */
#define false 0
/** Expands to _Bool. */
typedef _Bool bool;

#endif