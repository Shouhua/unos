#ifndef __STRING_H__
#define __STRING_H__

#include "stdint.h"

char *itoa(int32_t val, char *buf, int radix);
char *uitoa(uint32_t val, char *buf, int radix);
size_t strlen(const char *buf);
int printf(const char *format, ...);

void *memset(void *s, uint8_t c, size_t n);
void *memmove(void *dst, const void *src, size_t len);
void *memcpy(void *dst, void const *src, int n);
uint16_t *memsetw(uint16_t *dest, uint16_t val, uint32_t count);
uint32_t *memsetdw(uint32_t *dest, uint32_t val, uint32_t count);
int strncmp( const char* s1, const char* s2, int c );
int strcmp(const char *dst, char *src);
char *strncpy(char *destString, const char *sourceString,int maxLength);
char * strstr(const char *in, const char *str);
int strcpy(char *dst,const char *src);
void strcat(void *dest,const void *src);

#endif