#include "io.h"
#include "framebuffer.h"

/* fb_move_cursor
 * Move the cursor of the framebuffer to the given position
 *
 * @param pos the new position of the cursor
 */
void fb_move_cursor(unsigned short pos) 
{
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT, ((pos >> 8) & 0x00ff));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos & 0x00ff);
}

/** fb_write_cell
 *  Writes a character with the given foreground and backend to position i
 *  in the framebuffer.
 *  
 *  @param i  The location in the framebuffer
 *  @param c  The character
 *  @param fg The foreground color
 *  @param bg The background color
 */
void fb_write_cell(short i, char c, unsigned char fg, unsigned char bg)
{
    char *fb = (char *)0x000B8000;
    fb[i*2] = c;
    fb[i*2 + 1] = ((bg & 0x0f) << 4) | (fg & 0x0f);
}

void fb_write(char *buf) {
    unsigned int pos = 0;
    unsigned int i = 0;
    while(buf[i] != '\0') {
        fb_write_cell(pos++, buf[i++], FB_WHITE, FB_BLACK);
    }
    // for(i = 0; i < len; ++i) {
    //     fb_write_cell(pos++, buf[i], FB_WHITE, FB_BLACK);
    // }
    fb_move_cursor(i);
}