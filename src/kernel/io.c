#include "kernel/io.h"
#include "lib/stdint.h"

uint8_t inb(uint16_t port) {
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}
void outb(uint16_t port, uint8_t data) {
	asm volatile("outb %1, %0" : : "dN"(port), "a"(data));
}

uint16_t inw(uint16_t port) {
	uint16_t rv;
	asm volatile ("inw %1, %0" : "=a" (rv) : "dN" (port));
	return rv;
}
void outw(uint16_t port, uint16_t data) {
	asm volatile ("outw %1, %0" : : "dN" (port), "a" (data));
}

uint32_t inl(uint16_t port) {
	uint32_t rv;
	asm volatile ("inl %%dx, %%eax" : "=a" (rv) : "dN" (port));
	return rv;
}
void outl(uint16_t port, uint32_t data) {
	asm volatile ("outl %%eax, %%dx" : : "dN" (port), "a" (data));
}
