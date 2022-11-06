#ifndef __IO_H__
#define __IO_H__

#include "lib/stdint.h"

/** outb:
 *  Sends the given data to the given I/O port. Defined in io.s
 *
 *  @param port The I/O port to send the data to
 *  @param data The data to send to the I/O port
 */
void outb(uint16_t port, uint8_t data);

/*
* inb:
* Read a byte from an I/O port
*/
unsigned char inb(uint16_t port);

#endif
