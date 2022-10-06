#include "types.h"
#include "framebuffer.h"

char *itoa(int val, char *buf, int radix)
{
	u32 i = 0;
	u32 start = i;
	if (val < 0 && radix == 10)
	{
		buf[i++] = '-';
		start = i;
	}

	// if (radix == 16)
	// {
	// 	buf[i++] = '0';
	// 	buf[i++] = 'x';
	// 	start = i;
	// }

	int x = val;
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

char *uitoa(u32 val, char *buf, int radix)
{
	u32 i = 0;
	u32 start = i;

	if (radix == 16)
	{
		buf[i++] = '0';
		buf[i++] = 'x';
		start = i;
	}

	u32 x = val;
	do
	{
		u32 a = x % radix;
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
void *memset(void *s, int c, size_t n)
{
	char *mem = (char *)s;
	size_t i;
	for (i = 0; i < n; i++)
	{
		mem[i] = (u8)c;
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

int printf(const char *format, ...)
{
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
			case 'x':
				itoa(*((int *)arg++), buf, c == 'x' ? 16 : 10);
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