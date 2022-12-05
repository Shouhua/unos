#include "lib/stdint.h"
#include "kernel/framebuffer.h"
#include "lib/string.h"

char *itoa(int32_t val, char *buf, int radix)
{
	uint32_t i = 0;
	uint32_t start = i;
	if (val < 0 && radix == 10)
	{
		buf[i++] = '-';
		start = i;
	}

	int32_t x = val;
	do
	{
		int a = x % radix;
		if (a < 10)
			buf[i++] = a + '0';
		else
			buf[i++] = a + 'a' - 10;
	} while (x /= radix);

	char *s = buf + start;
	char *e = buf + (i - 1);

	while (s < e)
	{
		char t = *s;
		*s = *e;
		*e = t;
		s++;
		e--;
	}

	buf[i] = 0;
	return buf;
}

char *uitoa(uint32_t val, char *buf, int radix)
{
	uint32_t i = 0;
	uint32_t start = i;

	uint32_t x = val;
	do
	{
		uint32_t a = x % radix;
		if (a < 10)
			buf[i++] = a + '0';
		else
			buf[i++] = a + 'a' - 10;
	} while (x /= radix);

	char *s = buf + start;
	char *e = buf + (i - 1);

	while (s < e)
	{
		char t = *s;
		*s = *e;
		*e = t;
		s++;
		e--;
	}

	buf[i] = 0;
	return buf;
}

size_t strlen(const char *buf)
{
	unsigned int i = 0;
	while (buf[i] != 0)
		i++;
	return i;
}

// Terribly naive implementation of memset to get things compiling
// See http://www.xs-labs.com/en/blog/2013/08/06/optimising-memset/
// for more details
void *memset(void *s, uint8_t c, size_t n)
{
	uint8_t *mem = (uint8_t *)s;
	size_t i;
	for (i = 0; i < n; i++)
	{
		mem[i] = c;
	}
	return s;
}

void *memmove(void *dst, const void *src, size_t len)
{
	char *dstmem = (char *)dst;
	char *srcmem = (char *)src;
	size_t i;
	for (i = 0; i < len; i++)
	{
		dstmem[i] = srcmem[i];
	}
	return dstmem;
}

void *memcpy(void *dst, void const *src, int n)
{
	char * ret = dst;
	char * p = dst;
	const char * q = src;
	while (n--)
			*p++ = *q++;
	return ret;
}

int printf(const char *format, ...)
{
	scroll();
	char **arg = (char **)&format;
	int c;
	char buf[20];

	arg++;

	while ((c = *format++) != 0)
	{
		if (c != '%')
			fb_write_char(c);
		else
		{
			char *p, *p2;
			int pad0 = 0, pad = 0;

			c = *format++;
			if (c == '0')
			{
				pad0 = 1;
				c = *format++;
			}
			if (c >= '0' && c <= '9')
			{
				pad = c - '0';
				c = *format++;
			}
			switch (c)
			{
			case 'd':
			case 'i':
			case 'u':
				itoa(*((uint32_t *)arg++), buf, 10);
				p = buf;
				goto string;
				break;
			case 'x':
				uitoa(*((uint32_t *)arg++), buf, 16);
				p = buf;
				goto string;
				break;
			case 's':
				p = *arg++;
				if (!p)
					p = "(null)";
			string:
				for (p2 = p; *p2; p2++)
					;
				for (; p2 < p + pad; p2++)
					fb_write_char(pad0 ? '0' : ' ');
				while (*p)
					fb_write_char(*p++);
				break;
			default:
				fb_write_char(*((int *)arg++));
				break;
			}
		}
	}
	return 0;
}

/*
 * memset by word(16 bit)
 * */
uint16_t *memsetw(uint16_t *dest, uint16_t val, uint32_t count)
{
    uint16_t *temp = (uint16_t *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

/*
 * memset by double word(32 bit)
 * */
uint32_t *memsetdw(uint32_t *dest, uint32_t val, uint32_t count)
{
    uint32_t *temp = (uint32_t *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

// int strlen(const char * s) {
//     int len = 0;
//     while(*s++)
//         len++;
//     return len;
// }
char *strncpy(char *destString, const char *sourceString,int maxLength)
{
    unsigned count;

    if ((destString == (char *) NULL) || (sourceString == (char *) NULL))
    {
        return (destString = NULL);
    }

    if (maxLength > 255)
        maxLength = 255;

    for (count = 0; (int)count < (int)maxLength; count ++)
    {
        destString[count] = sourceString[count];

        if (sourceString[count] == '\0')
            break;
    }

    if (count >= 255)
    {
        return (destString = NULL);
    }

    return (destString);
}

int strcmp(const char *dst, char *src)
{
    int i = 0;

    while ((dst[i] == src[i])) {
        if (src[i++] == 0)
            return 0;
    }

    return 1;
}

char * strstr(const char *in, const char *str) {
    char c;
    uint32_t len;

    c = *str++;
    if (!c)
        return (char *) in;

    len = strlen(str);
    do {
        char sc;

        do {
            sc = *in++;
            if (!sc)
                return (char *) 0;
        } while (sc != c);
    } while (strncmp(in, str, len) != 0);

    return (char *) (in - 1);
}
int strcpy(char *dst,const char *src)
{
    int i = 0;
    while ((*dst++ = *src++) != 0)
        i++;
    return i;
}


void strcat(void *dest,const void *src)
{
    char * end = (char*)dest + strlen(dest);
    memcpy((char*)end,(char*)src,strlen((char*)src));
    end = end + strlen((char*)src);
    *end = '\0';
}

int strncmp( const char* s1, const char* s2, int c ) {
    int result = 0;

    while ( c ) {
        result = *s1 - *s2++;

        if ( ( result != 0 ) || ( *s1++ == 0 ) ) {
            break;
        }

        c--;
    }

    return result;
}