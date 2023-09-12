#include "md_api.h"
#include "md_math.h"
#include <stddef.h>

#include "maze.h"
#include "tiles.h"

#include "font8x8_basic.h"


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

const uint16_t pal[16] = { 0x0E00, 0x0222, 0x0444, 0x0666, 0x0888, 0x0AAA, 0x0CCC, 0x0EEE, 0x0000, 0x000E, 0x00E0, 0x00EE, 0x0E00, 0x0E0E, 0x0EE0, 0x0EEE, };

void interrupt_h() {
	asm("rte");
	// asm(".short 0x4E73"); // RTE - 0100 1110 0111 0011
}

void interrupt_v() {
	asm("move.b #1, vbi");
	asm("rte");
	// asm(".short 0x4E73"); // RTE - 0100 1110 0111 0011
}

static void draw_pad(uint8_t x,uint8_t y,uint16_t attr,uint16_t bits) {
	video_set_vram_write_addr(video_plane_w_addr(x,y));
	VDP_DATA_W = attr | (bits & JOYPAD_UP? 'U' : ' ') - video_first_char;
	VDP_DATA_W = attr | (bits & JOYPAD_DOWN? 'D' : ' ') - video_first_char;
	VDP_DATA_W = attr | (bits & JOYPAD_LEFT? 'L' : ' ') - video_first_char;
	VDP_DATA_W = attr | (bits & JOYPAD_RIGHT? 'R' : ' ') - video_first_char;
	VDP_DATA_W = attr | (bits & JOYPAD_A? 'A' : ' ') - video_first_char;
	VDP_DATA_W = attr | (bits & JOYPAD_B? 'B' : ' ') - video_first_char;
	VDP_DATA_W = attr | (bits & JOYPAD_C? 'C' : ' ') - video_first_char;
	VDP_DATA_W = attr | (bits & JOYPAD_START? 'S' : ' ') - video_first_char;
	VDP_DATA_W = attr | (bits & JOYPAD_X? 'X' : ' ') - video_first_char;
	VDP_DATA_W = attr | (bits & JOYPAD_Y? 'Y' : ' ') - video_first_char;
	VDP_DATA_W = attr | (bits & JOYPAD_Z? 'Z' : ' ') - video_first_char;
	VDP_DATA_W = attr | (bits & JOYPAD_MODE? 'M' : ' ') - video_first_char;
	VDP_DATA_W = attr | (bits & JOYPAD_6? '6' : '3') - video_first_char;
}

extern char _bstart[], _bend[];

void Main();

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

void Main() {
	uint8_t flags = REG_VERSION_B;
	video_init(PLANE_SIZE_32_32,0xC000);
	video_config_window(RIGHT_OF_X_SPLIT | 15,BELOW_Y_SPLIT | 26);

	// asm volatile("move #$2000,sr");

	video_set_vram_write_addr(0);
	video_upload_bitmap_font(&font8x8_basic[0][0],64,15,14,32);

	// video_draw_string(video_plane_a_addr(10,10),text_attr,flags & REG_VERSION_PAL?"PAL ":"NTSC");
	// video_draw_string(video_plane_a_addr(10,12),text_attr,flags & REG_VERSION_OVERSEAS?"OVERSEAS":"DOMESTIC");

	video_draw_string(video_plane_w_addr(32,0),video_window_attr,"STR:18");
	video_draw_string(video_plane_w_addr(32,1),video_window_attr,"DEX:11");
	video_draw_string(video_plane_w_addr(32,2),video_window_attr,"CON:12");
	video_draw_string(video_plane_w_addr(32,3),video_window_attr,"WIS:08");
	video_draw_string(video_plane_w_addr(32,4),video_window_attr,"INT:09");
	video_draw_string(video_plane_w_addr(32,5),video_window_attr,"HP:010");

	video_upload_palette(0,tiles_palette_0);
	video_upload_palette(1,tiles_palette_1);
	video_upload_palette(2,tiles_palette_2);
	video_upload_palette(3,tiles_palette_3);

	video_set_vram_write_addr(0x0800);
	uint8_t palettes[32];
	const uint32_t *b = tiles_directory + tiles_walls_2_0;
	for (int i=0; i<27; i++) {
		palettes[i] = b[i] >> 29;
		// 68000 has a 24 bit data bus, upper eight bits aren't used
		// would ordinarity use lower bits but linker doesn't align to 32 bit boundaries
		video_upload_sprite((uint32_t*)b[i],9);
	}
	uint32_t d = tiles_directory[tiles_decor_2_0];
	palettes[27] = (d >> 29) | 4;
	video_upload_sprite((uint32_t*)d,9);
	for (int i=0; i<8*9; i++)
		VDP_DATA_L = 0;
	b = tiles_directory + tiles_decor_1_0;
	for (int i=0; i<4; i++) {
		palettes[28+i] = (b[i] >> 29) | 4; // spider webs have priority
		video_upload_sprite((uint32_t*)b[i],9);
	}

	maze_init(64,palettes);
	maze_draw(0,0);

	video_enable();

	uint16_t n = 0;
	uint32_t elapsed = 0;
	uint32_t step = (flags & REG_VERSION_PAL)? 65536/50 : 65536/60;
	uint32_t next = 0;
	uint32_t off_x = 0, off_y = 0;
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
		video_draw_string(video_plane_w_addr(32,10),video_window_attr,timer);

		video_set_vram_write_addr(0xF000);
		uint16_t ti = modulo(elapsed >> 12, tiles_chars_21_17-tiles_chars_0_0+1) + tiles_chars_0_0;
		VDP_DATA_W = 128 + 50 + ((elapsed >> 13) & 127); // y
		VDP_DATA_W = 0x0A00;
		const int sprite_addr = 0xC800;
		VDP_DATA_W = ((tiles_directory[ti] >> 16) & 0x6000) | (sprite_addr >> 5);
		VDP_DATA_W = 128 + 20 + ((elapsed >> 12) & 255); // x
		video_set_vram_write_addr(sprite_addr);
		video_upload_sprite((uint32_t*)tiles_directory[ti],9);

		video_set_vram_write_addr(0xF800);
		VDP_DATA_L = (uint16_t)(-off_x) | (-off_x << 16);
		
		VDP_CTRL_L = 0x40000010;
		VDP_DATA_L = off_y | (off_y << 16);

		uint16_t pad0 = joypad_read(0), pad1 = joypad_read(1);
		if (pad0 & JOYPAD_START)
			elapsed = 0;
		uint8_t d = (pad0 & JOYPAD_A)? 8 : 1;
		uint32_t new_off_x = (pad0 & JOYPAD_RIGHT) && (off_x+d < (MAZE_SIZE * 3 - 40) * 8)? off_x + d :
			(pad0 & JOYPAD_LEFT) && off_x>=d? off_x - d : off_x;
		uint32_t new_off_y = (pad0 & JOYPAD_DOWN) && (off_y+d < (MAZE_SIZE * 3 - 28) * 8)? off_y + d :
			(pad0 & JOYPAD_UP) && off_y>=d ? off_y - d : off_y;
		if ((new_off_x>>3) > (off_x>>3))
			maze_new_right_column(new_off_x>>3,off_y>>3);
		else if ((new_off_x>>3) < (off_x>>3))
			maze_new_left_column(new_off_x>>3,off_y>>3);
		if ((new_off_y>>3) > (off_y>>3))
			maze_new_bottom_row(new_off_x>>3,new_off_y>>3);
		else if ((new_off_y>>3) < (off_y>>3))
			maze_new_top_row(new_off_x>>3,new_off_y>>3);
		off_y = new_off_y;
		off_x = new_off_x;
			
		draw_pad(0,26,video_window_attr,pad0);
		draw_pad(0,27,video_window_attr,pad1);
		// video_set_palette_entry(1, n & 0xEEE);
		// n += 0x010101;
	}
}
