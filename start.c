#include "md_api.h"

// Writes to VRAM starting at 0xD040 (the window area)

#define TRAP(code) \
	VDP_CTRL_L = 0x50400003; \
	VDP_DATA_W = (uint8_t)(code>>24) - 32; \
	VDP_DATA_W = (uint8_t)(code>>16) - 32; \
	VDP_DATA_W = (uint8_t)(code>>8) - 32; \
	VDP_DATA_W = (uint8_t)(code) - 32; \
	while(1)

void crash() {
	TRAP('UNKN');
}

void bus_error() {
	TRAP('BUS ');
}

void address_error() {
	TRAP('ADDR');
}

void illegal_inst() {
	TRAP('ILLE');
}

void div_zero() {
	TRAP('DIV ');
}

void chk_exception() {
	TRAP('CHKE');
}

void trapv_exception() {
	TRAP('TRPV');
}

volatile uint8_t vbi;

volatile uint32_t __halt1, __halt2;

void interrupt_h() {
	asm("rte");
	// asm(".short 0x4E73"); // RTE - 0100 1110 0111 0011
}

void interrupt_v() {
	asm("move.b #1, vbi");
	asm("rte");
	// asm(".short 0x4E73"); // RTE - 0100 1110 0111 0011
}

extern char _bstart[], _bend[];

extern void Main();

void _start() {
	// MOVE to SR is 0100 0110 11xx xxxx; IM EA is 111 100; Final result is 0x46FC
	asm(".long 0x46FC2700"); // move #0x2700, sr
	// asm volatile("move #$2700,sr");
	if (REG_VERSION_B & REG_VERSION_TMSS)
		REG_TMSS_L = 'SEGA';

	// Clear BSS
	char *b = _bstart;
	while (b < _bend)
		*b++ = 0;

	Main();
}
