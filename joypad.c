#include "md_api.h"

uint8_t joypad_read(int which) {
	REG_CTRL_B(which) = 0x40;
	volatile uint8_t *data = &REG_DATA_B(which);
	*data = 0x40; asm("nop"); asm("nop");
	uint8_t ret = *data & 0x3F;
	*data = 0x00; asm("nop"); asm("nop");
	ret |= (*data & 0x30) << 2;

	/* *data = 0x40; asm("nop"); asm("nop");
	*data = 0x00; asm("nop"); asm("nop");
	*data = 0x40; asm("nop"); asm("nop");
	return (uint16_t)ret | (uint16_t)((*data & 0xF) << 8); */

	return ~ret;
}
