#include "md_api.h"
#include "md_math.h"
#include <stddef.h>

#include "maze.h"
#include "tiles.h"

#include "font8x8_basic.h"

void crash() {
	while (1);
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

static void draw_pad(const char *tag,uint8_t x,uint8_t y,uint16_t attr,uint16_t bits) {
	video_draw_string(video_plane_a_addr(x,y),attr,tag);
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
	video_init(PLANE_SIZE_64_32);
	video_config_window(RIGHT_OF_X_SPLIT | 17,0);

	// asm volatile("move #$2000,sr");

	video_set_vram_write_addr(0);
	video_upload_bitmap_font(&font8x8_basic[0][0],64,0x0,0x7,32);

	const uint16_t text_attr = NT_PRIORITY | NT_PALETTE_0;
	// video_draw_string(video_plane_a_addr(10,10),text_attr,flags & REG_VERSION_PAL?"PAL ":"NTSC");
	// video_draw_string(video_plane_a_addr(10,12),text_attr,flags & REG_VERSION_OVERSEAS?"OVERSEAS":"DOMESTIC");

	video_draw_string(video_plane_w_addr(34,0),0,"STR:18");
	video_draw_string(video_plane_w_addr(34,1),0,"DEX:11");
	video_draw_string(video_plane_w_addr(34,2),0,"CON:12");
	video_draw_string(video_plane_w_addr(34,3),0,"WIS:08");
	video_draw_string(video_plane_w_addr(34,4),0,"INT:09");
	video_draw_string(video_plane_w_addr(34,6),0,"HP:010");

	video_upload_palette(0,tiles_palette_0);
	video_upload_palette(1,tiles_palette_1);
	video_upload_palette(2,tiles_palette_2);
	video_upload_palette(3,tiles_palette_3);
	video_set_vram_write_addr(0x4000);

	uint8_t palettes[27];
	const uint32_t *b = tiles_directory + tiles_walls_2_0;
	for (int i=0; i<27; i++) {
		palettes[i] = b[i] >> 29;
		// 68000 has a 24 bit data bus, upper eight bits aren't used
		// would ordinarity use lower bits but linker doesn't align to 32 bit boundaries
		video_upload_sprite((uint32_t*)b[i],9);
	}
	
	maze_init(512,palettes);
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
		video_draw_string(video_plane_a_addr(10,14),text_attr,timer);

		video_set_vram_write_addr(0xF000);
		uint16_t ti = modulo(elapsed >> 12, tiles_chars_21_17-tiles_chars_0_0+1) + tiles_chars_0_0;
		VDP_DATA_W = 128 + 50 + ((elapsed >> 13) & 127);
		VDP_DATA_W = 0x0A00;
		VDP_DATA_W = ((tiles_directory[ti] >> 16) & 0x6000) | 256;
		VDP_DATA_W = 128 + 20 + ((elapsed >> 12) & 255);
		video_set_vram_write_addr(0x2000);
		video_upload_sprite((uint32_t*)tiles_directory[ti],9);

		video_set_vram_write_addr(0xF802);
		VDP_DATA_W = -off_x;
		
		VDP_CTRL_L = 0x40020010;
		VDP_DATA_W = off_y;

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
			
		draw_pad("ONE ", 4,16,text_attr, pad0);
		draw_pad("TWO ", 4,18,text_attr, pad1);
		// video_set_palette_entry(1, n & 0xEEE);
		// n += 0x010101;
	}
}
	
