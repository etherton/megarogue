#include "md_api.h"
#include "md_math.h"
#include <stddef.h>

#include "maze.h"

#include "font8x8_basic.h"

void crash() {
	while (1);
}

volatile uint8_t vbi;

volatile uint32_t __halt1, __halt2;

const uint16_t pal[16] = { 0x0E00, 0x0222, 0x0444, 0x0666, 0x0888, 0x0AAA, 0x0CCC, 0x0EEE, 0x0000, 0x000E, 0x00E0, 0x00EE, 0x0E00, 0x0E0E, 0x0EE0, 0x0EEE, };

struct directory { const uint32_t *tilePtr; const uint16_t *palPtr; };
extern const struct directory char_directory[], bg_directory[];;
extern const uint16_t char_directory_count;
extern const uint16_t bg_directory_count;

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
	VDP_DATA_W = attr | (bits & JOYPAD_UP? 'U' : ' ');
	VDP_DATA_W = attr | (bits & JOYPAD_DOWN? 'D' : ' ');
	VDP_DATA_W = attr | (bits & JOYPAD_LEFT? 'L' : ' ');
	VDP_DATA_W = attr | (bits & JOYPAD_RIGHT? 'R' : ' ');
	VDP_DATA_W = attr | (bits & JOYPAD_A? 'A' : ' ');
	VDP_DATA_W = attr | (bits & JOYPAD_B? 'B' : ' ');
	VDP_DATA_W = attr | (bits & JOYPAD_C? 'C' : ' ');
	VDP_DATA_W = attr | (bits & JOYPAD_START? 'S' : ' ');
	VDP_DATA_W = attr | (bits & JOYPAD_X? 'X' : ' ');
	VDP_DATA_W = attr | (bits & JOYPAD_Y? 'Y' : ' ');
	VDP_DATA_W = attr | (bits & JOYPAD_Z? 'Z' : ' ');
	VDP_DATA_W = attr | (bits & JOYPAD_MODE? 'M' : ' ');
	VDP_DATA_W = attr | (bits & JOYPAD_6? '6' : '3');
}

extern char _bstart[], _bend[];

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

	uint8_t flags = REG_VERSION_B;
	video_init(PLANE_SIZE_64_32);
	video_config_window(RIGHT_OF_X_SPLIT | 17,0);

	// asm volatile("move #$2000,sr");
	video_upload_palette(0, pal);

	video_set_vram_write_addr(0);
	video_upload_bitmap_font(&font8x8_basic[0][0],8 * 128,0x0,0x7);

	const uint16_t text_attr = NT_PRIORITY | NT_PALETTE_0;
	video_draw_string(video_plane_a_addr(10,10),text_attr,flags & REG_VERSION_PAL?"PAL ":"NTSC");
	video_draw_string(video_plane_a_addr(10,12),text_attr,flags & REG_VERSION_OVERSEAS?"OVERSEAS":"DOMESTIC");

	video_draw_string(video_plane_w_addr(34,0),0,"STR:18");
	video_draw_string(video_plane_w_addr(34,1),0,"DEX:11");
	video_draw_string(video_plane_w_addr(34,2),0,"CON:12");
	video_draw_string(video_plane_w_addr(34,3),0,"WIS:08");
	video_draw_string(video_plane_w_addr(34,4),0,"INT:09");
	video_draw_string(video_plane_w_addr(34,6),0,"HP:010");

	video_set_vram_write_addr(0x4000);
	for (int i=0; i<27; i++)
		video_upload_sprite(bg_directory[i+27*2].tilePtr,9);
	video_upload_palette(3,bg_directory[27*2].palPtr);
	
	maze_init();
	maze_draw(0,0);

	video_enable();
	asm(".long 0x46FC2000"); // move #0x2000, sr

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
		VDP_DATA_W = 128 + 50 + ((elapsed >> 13) & 127);
		VDP_DATA_W = 0x0A00;
		VDP_DATA_W = NT_PALETTE_1 | 256;
		VDP_DATA_W = 128 + 20 + ((elapsed >> 12) & 255);
		uint16_t ti = modulo(elapsed >> 12, char_directory_count);
		video_upload_palette(1,char_directory[ti].palPtr);
		video_set_vram_write_addr(0x2000);
		video_upload_sprite(char_directory[ti].tilePtr,9);

		video_set_vram_write_addr(0xF802);
		VDP_DATA_W = -off_x;
		
		VDP_CTRL_L = 0x40020010;
		VDP_DATA_W = off_y;

		uint16_t pad0 = joypad_read(0), pad1 = joypad_read(1);
		if (pad0 & JOYPAD_START)
			elapsed = 0;
		uint32_t new_off_x = (pad0 & JOYPAD_RIGHT) && (off_x < (MAZE_SIZE * 3 - 40) * 8 -1)? off_x + 1 :
			(pad0 & JOYPAD_LEFT) && off_x? off_x - 1 : off_x;
		uint32_t new_off_y = (pad0 & JOYPAD_DOWN) && (off_y < (MAZE_SIZE * 3 - 28) * 8 -1)? off_y + 1 :
			(pad0 & JOYPAD_UP) && off_y ? off_y - 1 : off_y;
		if (new_off_x > off_x && (new_off_x & 7)==0)
			maze_new_right_column(new_off_x>>3,new_off_y>>3);
		else if (new_off_x < off_x && (new_off_x & 7)==7)
			maze_new_left_column(new_off_x>>3,new_off_y>>3);
		if (new_off_y > off_y && (new_off_y & 7)==0)
			maze_new_bottom_row(new_off_x>>3,new_off_y>>3);
		else if (new_off_y < off_x && (new_off_y & 7)==7)
			maze_new_top_row(new_off_x>>3,new_off_y>>3);
		off_x = new_off_x;
		off_y = new_off_y;
			
		draw_pad("ONE ", 4,16,text_attr, pad0);
		draw_pad("TWO ", 4,18,text_attr, pad1);
		// video_set_palette_entry(1, n & 0xEEE);
		// n += 0x010101;
	}
}
	
