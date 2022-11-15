#ifndef __STRING_H__
#define __STRING_H__

#include "stdint.h"

char *itoa(int val, char *buf, uint32_t radix);
char *uitoa(uint32_t val, char *buf, uint32_t radix);
size_t strlen(const char *buf);
int printf(const char *format, ...);

void *memset(void *s, int c, size_t n);
void *memmove(void *dst, const void *src, size_t len);
void *memcpy(void *dst, void const *src, int n);

#endif