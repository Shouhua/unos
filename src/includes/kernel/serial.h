#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "lib/stdint.h"

#define SERIAL_COM1_BASE                0x3F8

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

#define SERIAL_LINE_ENABLE_DLAB         0x80

void serial_configure_baud_rate(uint16_t com, uint16_t divisor);
void serial_configure_line(uint16_t com);

 /** serial_is_transmit_fifo_empty:
  *  Checks whether the transmit FIFO queue is empty or not for the given COM
  *  port.
  *
  *  @param  com The COM port
  *  @return 0 if the transmit FIFO queue is not empty
  *          1 if the transmit FIFO queue is empty
  */
int serial_is_transmit_fifo_empty(uint32_t com);

void serial_write(char *buf, uint32_t len);

#endif