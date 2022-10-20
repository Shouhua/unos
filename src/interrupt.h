#ifndef __INTERRUPT_H_
#define __INTERRUPT_H_

#include "types.h"

typedef struct idt_entry_t {
  u16 offset_high; // 17 - 31?
  u8 handle_present:1; // 16
  u8 privilege_level:2; // 14 - 15
  u8 unused:1; // 13
  u8 gate_size:1; // 12
  u16 reserved:11; // 0 - 11
  u16 segment_selector; // 15 - 31
  u16 offset_low; // 0 - 15
} idt_entry_t;


/**
 * Accepts interrupt_t struct and converts it an IDT entry
 */
void create_idt_entry() {

}
#endif /* __INTERRUPT_H_ */