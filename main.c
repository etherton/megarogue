#include "md_api.h"
#include "md_math.h"
#include <stddef.h>

#include "maze.h"
#include "tiles.h"

#include "huffman_decode.h"

#include "font8x8_basic.h"

extern volatile uint8_t vbi;
extern volatile uint16_t hblanks;

extern const int8_t headerTree[];

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

void video_decompress_sprite_3x3(const uint16_t *bitstream) {
	uint32_t sprite[72];
	huffman_decode(headerTree,NSYMS,bitstream,(uint8_t*)sprite,24*24/2);
	video_upload_sprite(sprite,9);
}

const char *to_string_5(uint16_t e) {
	static char str[6];
	str[5] = 0;
	uint32_t v = div_mod(e,10); str[4] = (v>>16)+'0';
	v = div_mod((uint16_t)v,10); str[3] = (v>>16)+'0';
	v = div_mod((uint16_t)v,10); str[2] = (v>>16)+'0';
	v = div_mod((uint16_t)v,10); str[1] = (v>>16)+'0';
	v = div_mod((uint16_t)v,10); str[0] = (v>>16)+'0';
	return str;
}

const char *to_string_3(uint16_t e) {
	static char str[4];
	str[3] = 0;
	uint32_t v = div_mod(e,10); str[2] = (v>>16)+'0';
	v = div_mod((uint16_t)v,10); str[1] = (v>>16)+'0';
	v = div_mod((uint16_t)v,10); str[0] = (v>>16)+'0';
	return str;
}

void Main() {
	uint8_t flags = REG_VERSION_B;
	video_init(PLANE_SIZE_32_32,0xC000);
	video_config_window(RIGHT_OF_X_SPLIT | 15,BELOW_Y_SPLIT | 26);

	// asm volatile("move #$2000,sr");

	video_set_vram_write_addr(0x0800);
	uint8_t palettes[32];
	const uint32_t *b = tiles_directory + tiles_walls_2_0;
	for (int i=0; i<27; i++) {
		palettes[i] = b[i] >> 29;
		// 68000 has a 24 bit data bus, upper eight bits aren't used
		// would ordinarity use lower bits but linker doesn't align to 32 bit boundaries
		video_decompress_sprite_3x3((uint16_t*)b[i]);
	}
	b = tiles_directory + tiles_decor_2_0;
	palettes[27] = (b[0] >> 29) | 4; // doors have priority
	video_decompress_sprite_3x3((uint16_t*)b[0]);
	for (int i=0; i<8*9; i++)
		VDP_DATA_L = 0;
	b = tiles_directory + tiles_decor_1_0;
	for (int i=0; i<4; i++) {
		palettes[28+i] = (b[i] >> 29) | 4; // spider webs have priority
		video_decompress_sprite_3x3((uint16_t*)b[i]);
	}

	video_set_vram_write_addr(0);
	video_upload_bitmap_font(&font8x8_basic[0][0],64,15,14,32);

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

	maze_init(64,palettes);
	maze_draw(0,0);

	video_enable();

	uint16_t n = 0;
	uint32_t elapsed = 0;
	uint32_t step = (flags & REG_VERSION_PAL)? 65536/50 : 65536/60;
	uint32_t next = 0;
	uint32_t off_x = 0, off_y = 0;
	uint16_t prevTi = 0xFFFF;
	int8_t bodyTree[64];
	uint32_t sprite[72];
	int state = 0;
	size_t progress;
	uint16_t *src;
	uint16_t profile1=0, profile2=0, profile3=0;
	while (1) {
		elapsed += step;

		uint16_t ti = modulo(elapsed >> 15, tiles_chars_21_17-tiles_chars_0_0+1) + tiles_chars_0_0;
		if (ti != prevTi)  {
			state = 1;
			prevTi = ti;
			src = (uint16_t*)tiles_directory[ti];
		}
		if (state == 1) {
			profile1 = hblanks;
			progress = huffman_decode_header(bodyTree,headerTree,NSYMS,src);
			progress = huffman_decode_body(bodyTree,src,(uint8_t*)sprite,24*6,progress);
			state = 2;
			profile1 = hblanks - profile1;
		}
		else if (state == 2) {
			profile2 = hblanks;
			progress = huffman_decode_body(bodyTree,src,(uint8_t*)sprite,24*15,progress);
			state = 3;
			profile2 = hblanks - profile2;
		}
		else if (state == 3) {
			profile3 = hblanks;
			progress = huffman_decode_body(bodyTree,src,(uint8_t*)sprite,24*24,progress);
			state = 0;
			profile3 = hblanks - profile3;
		}

		while (!vbi);
		vbi = 0;
		video_draw_string(video_plane_w_addr(34,10),video_window_attr,to_string_3(elapsed>>16));
		video_draw_string(video_plane_w_addr(34,11),video_window_attr,to_string_3(profile1));
		video_draw_string(video_plane_w_addr(34,12),video_window_attr,to_string_3(profile2));
		video_draw_string(video_plane_w_addr(34,13),video_window_attr,to_string_3(profile3));

		video_set_vram_write_addr(0xF000);
		VDP_DATA_W = 128 + 50 + ((elapsed >> 13) & 127); // y
		VDP_DATA_W = 0x0A00;
		const int sprite_addr = 0xC800;
		VDP_DATA_W = ((tiles_directory[ti] >> 16) & 0x6000) | (sprite_addr >> 5);
		VDP_DATA_W = 128 + 20 + ((elapsed >> 12) & 255); // x
		video_set_vram_write_addr(sprite_addr);

		video_upload_sprite(sprite,9);

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
	}
}
