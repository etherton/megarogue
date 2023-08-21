#include "md_api.h"

#include "font8x8_basic.h"

void crash() {
	while (1);
}

volatile uint8_t vbi;

const uint16_t pal[] = { 
	0x0000, 0x0222, 0x0444, 0x0666, 0x0888, 0x0AAA, 0x0CCC, 0x0EEE, 0x0000, 0x000E, 0x00E0, 0x00EE, 0x0E00, 0x0E0E, 0x0EE0, 0x0EEE,
	0x0E00, 0x0E00, 0x0E00, 0x0E00, 0x0E00, 0x0E00, 0x0E00, 0x0E00, 0x0E00, 0x0E00, 0x0E00, 0x0E00, 0x0E00, 0x0E00, 0x0E00, 0x0E00,
 };

void interrupt_h() {
	asm("rte");
	// asm(".short 0x4E73"); // RTE - 0100 1110 0111 0011
}

void interrupt_v() {
	asm("move.b #1, vbi");
	asm("rte");
	// asm(".short 0x4E73"); // RTE - 0100 1110 0111 0011
}

static void draw_pad(const char *tag,uint8_t x,uint8_t y,uint16_t attr,uint16_t bits) {
	video_draw_string(video_plane_a_addr(x,y),attr,tag);
	VDP_DATA_W = bits & JOYPAD_UP? 'U' : ' ';
	VDP_DATA_W = bits & JOYPAD_DOWN? 'D' : ' ';
	VDP_DATA_W = bits & JOYPAD_LEFT? 'L' : ' ';
	VDP_DATA_W = bits & JOYPAD_RIGHT? 'R' : ' ';
	VDP_DATA_W = bits & JOYPAD_A? 'A' : ' ';
	VDP_DATA_W = bits & JOYPAD_B? 'B' : ' ';
	VDP_DATA_W = bits & JOYPAD_C? 'C' : ' ';
	VDP_DATA_W = bits & JOYPAD_START? 'S' : ' ';
}

void _start() {
	// MOVE to SR is 0100 0110 11xx xxxx; IM EA is 111 100; Final result is 0x46FC
	asm(".long 0x46FC2700"); // move #0x2700, sr
	// asm volatile("move #$2700,sr");
	if (REG_VERSION_B & REG_VERSION_TMSS)
		REG_TMSS_L = 'SEGA';
	uint8_t flags = REG_VERSION_B;
	video_init();
	// asm volatile("move #$2000,sr");
	video_load_palette(0, pal, 32);
	// video_set_palette_entry(0, 0x000E);
	// video_set_palette_entry(1, 0x0EEE);
	video_set_vram_write_addr(0);
/* for (int i=0; i<4; i++) {
	// Tile 0 first row
	VDP_DATA_W = 0x0123;
	VDP_DATA_W = 0x4567;
	// Second row
	VDP_DATA_W = 0x1234;
	VDP_DATA_W = 0x5670;
	// Third row
	VDP_DATA_W = 0x2345;
	VDP_DATA_W = 0x6701;
	// Fourth row
	VDP_DATA_W = 0x3456;
	VDP_DATA_W = 0x7012;
} */

	video_upload_bitmap_font(&font8x8_basic[0][0],8 * 128,0x0,0x7);

	// Plane A, Palette 0
	/* video_set_vram_write_addr(0xC000 + 60);
	VDP_DATA_W = 'S';
	VDP_DATA_W = 'e';
	VDP_DATA_W = 'g';
	VDP_DATA_W = 'a'; */

	video_draw_string(video_plane_a_addr(10,10),0,flags & REG_VERSION_PAL?"PAL ":"NTSC");
	video_draw_string(video_plane_a_addr(10,12),0,flags & REG_VERSION_OVERSEAS?"OVERSEAS":"DOMESTIC");

	// Plane B, Palette 1
	/*video_set_vram_write_addr(0xE000 + 62);
	VDP_DATA_W = 'X' | 0x2000; //  | 0x8000;
	*/

	video_enable();
	asm(".long 0x46FC2000"); // move #0x2000, sr

	uint16_t n = 0;
	uint32_t elapsed = 0;
	uint32_t step = (flags & REG_VERSION_PAL)? 65536/50 : 65536/60;
	while (1) {
		elapsed += step;
		char timer[6];
		uint16_t e = elapsed >> 16;
		timer[5] = 0;
		timer[4] = (e % 10) + '0'; e /= 10;
		timer[3] = (e % 10) + '0'; e /= 10;
		timer[2] = (e % 10) + '0'; e /= 10;
		timer[1] = (e % 10) + '0'; e /= 10;
		timer[0] = (e % 10) + '0';

		while (!vbi);
		vbi = 0;
		video_draw_string(video_plane_a_addr(10,14),0x2000,timer);
		uint16_t pad0 = joypad_read(0), pad1 = joypad_read(1);
		if (pad0 & JOYPAD_START)
			elapsed = 0;
		draw_pad("ONE ", 10,16,0, pad0);
		draw_pad("TWO ", 10,18,0, pad1);
		// video_set_palette_entry(1, n & 0xEEE);
		// n += 0x010101;
	}
}
	
