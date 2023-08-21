#include "md_api.h"

// https://segaretro.org/Six_Button_Control_Pad_(Mega_Drive)
// https://github.com/Stephane-D/SGDK/blob/master/src/joy.c

static uint16_t phase(volatile uint8_t *data) {
	uint16_t val;
	*data = 0;
	asm volatile("nop");
	asm volatile("nop");
	val = *data;
	*data = 0x40;
	val <<= 8;
	val |= *data;
	return val;
}

uint16_t joypad_read(int which) {
	REG_CTRL_B(which) = 0x40;
	volatile uint8_t *data = &REG_DATA_B(which);
	uint16_t v1 = phase(data);
	uint16_t val = phase(data);
	uint16_t v2 = phase(data);
	val = phase(data);

	if ((v2 & 0xF00) || (!(val & 0x0C00))) // 3 button?
		v2 = 0xF00;
	else
		v2 = ((v2 & 0xF) << 8) | JOYPAD_6;
	val = ((v1 & 0x3000) >> 6) | (v1 & 0x3F);
	val |= v2;
	val ^= 0xFFF;
	return val;
}
