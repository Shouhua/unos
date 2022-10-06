#ifndef __STRING_H__
#define __STRING_H__

#include "types.h"

char *itoa(int val, char *buf, u32 radix);
char *uitoa(u32 val, char *buf, u32 radix);
size_t strlen(const char *buf);
int printf(const char *format, ...);

void *memset(void *s, int c, size_t n);
void *memmove(void *dst, const void *src, size_t len);

#endif