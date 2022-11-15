#include "lib/stdint.h"
#include "kernel/io.h"
#include "kernel/framebuffer.h"

#define FB_WIDTH 80
#define FB_HEIGHT 25

static uint32_t fb_pos_x = 0;
static uint32_t fb_pos_y = 0;

static uint8_t current_fg = FB_WHITE;
static uint8_t current_bg = FB_BLACK;
static uint8_t *framebuffer_addr = (uint8_t *)0x000B8000;

void fb_set_buffer(uint8_t *addr) 
{
	framebuffer_addr = addr;
}

void fb_set_color(uint8_t fg, uint8_t bg)
{
	current_fg = fg;
	current_bg = bg;
}

void fb_reset_color()
{
	current_fg = FB_WHITE;
	current_bg = FB_BLACK;
}

/**
 * fb_write_cell:
 * Writes a character with the given foreground and background to position i
 * within the frame buffer.
 * fb and bg must be between 0 and 15
 * i must be between 0 and 80*25 = 2000
 */
void fb_write_cell(int16_t i, int8_t c)
{
	uint8_t *fb = (uint8_t *)framebuffer_addr;
	fb[i * 2] = c;
	fb[i * 2 + 1] = ((current_bg & 0x0f) << 4) | (current_fg & 0x0f);
}

void fb_move_cursor(uint16_t pos)
{
	outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
	outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
	outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
	outb(FB_DATA_PORT, pos & 0x00FF);
}

void fb_write(int8_t *buf, uint32_t len)
{
	uint32_t i;
	for (i = 0; i < len; i++)
	{
		char c = buf[i];
		fb_write_char(c);
	}
}

void fb_write_str(char *buf)
{
	uint32_t i = 0;
	while (buf[i] != 0)
	{
		char c = buf[i];
		fb_write_char(c);
		i++;
	}
}

void fb_write_char(char c)
{
	uint16_t pos;
	if (c == '\n' || c == '\r')
	{
		fb_pos_y++;
		fb_pos_x = 0;
		pos = fb_pos_x + (fb_pos_y * FB_WIDTH);
		fb_move_cursor(pos);
	}
	else if(c == '\t') // 只是右移4位简单情况，没有考虑换行等特殊情况
	{ 
		fb_pos_x += 4;
		pos = fb_pos_x + (fb_pos_y * FB_WIDTH);
		fb_move_cursor(pos);
	}
	else
	{
		pos = fb_pos_x + (fb_pos_y * FB_WIDTH);
		fb_write_cell(pos, c);
		fb_move_cursor(pos + 1);
		fb_pos_x++;
	}

	// handle position at end of line
	if (fb_pos_x == FB_WIDTH)
	{
		fb_pos_y++;
		fb_pos_x = 0;
	}
}

void fb_clear()
{
	fb_pos_x = 0;
	fb_pos_y = 0;

	int i;
	for (i = 0; i < FB_WIDTH * FB_HEIGHT; i++)
	{
		fb_write_cell(i, ' ');
	}
}