#include "md_api.h"

// https://md.railgun.works/index.php?title=VDP

uint8_t video_plane_shift;
uint8_t video_plane_width, video_plane_width_mask;
uint8_t video_plane_height, video_plane_height_mask;

const uint8_t plane_shifts[] = { 5, 6, 0, 7 }; // 32,64,128

uint16_t video_window_attr;

void video_init(uint8_t plane_size,uint16_t attr) {
	VDP_CTRL_W = 0x8004; // No HBI, no HV latch
	VDP_CTRL_W = 0x8134; // No display, VBI, DMA OK, V28
	VDP_CTRL_W = 0x8200 | (0xC000 >> 10); // Plane A: $C000
	VDP_CTRL_W = 0x8300 | (0xD000 >> 10); // Window:  $D000
	VDP_CTRL_W = 0x8400 | (0xE000 >> 13); // Plane B: $E000
	VDP_CTRL_W = 0x8500 | (0xF000 >> 9);  // Sprites: $F000
	VDP_CTRL_W = 0x8700; // BG color: palette 0, index 0
	VDP_CTRL_W = 0x8B00; // No IRQ2, full scrolling
	VDP_CTRL_W = 0x8C81; // H40, no S/H, no interlace
	VDP_CTRL_W = 0x8D00 | (0xF800 >> 10); // Hscroll: $F800
	VDP_CTRL_W = 0x8F02; // Autoincrement: 2 bytes
	VDP_CTRL_W = 0x9000 | plane_size;
	VDP_CTRL_W = 0x9100; // Hide window plane
	VDP_CTRL_W = 0x9200; // Hide window plane

	// shift in bytes per line
	video_plane_shift = plane_shifts[plane_size & 3] + 1;
	video_plane_width = 1 << plane_shifts[plane_size & 3];
	video_plane_width_mask = video_plane_width-1;
	video_plane_height = 1 << plane_shifts[plane_size >> 4];
	video_plane_height_mask = video_plane_height-1;
	video_window_attr = attr;

	// Clear first long of VSRAM
	VDP_CTRL_L = 0x40000010;
	VDP_DATA_L = 0;
	// Clear all of VRAM (except window to priority)
	video_set_vram_write_addr(0);
	int c = 0xD000 >> 2;
	while (c--)
		VDP_DATA_L = 0;
	c = 0x1000 >> 2;
	while (c--)
		VDP_DATA_L = attr | (attr << 16);
	c = 0x2000 >> 2;
	while (c--)
		VDP_DATA_L = 0;
	c = 0x2000 >> 2;
}

void video_config_window(uint8_t h,uint8_t v) {
	VDP_CTRL_W = 0x9100 | h;
	VDP_CTRL_W = 0x9200 | v;
}

void video_enable() {
	VDP_CTRL_W = 0x8174;	// Enable display
	asm(".long 0x46FC2000"); // move #0x2000, sr
}

void video_upload_palette(uint8_t baseAddr,const uint16_t *palette) {
	md_assert(baseAddr < 4);
	const uint32_t *p = (const uint32_t*) palette;
	VDP_CTRL_L = 0xC0000000 + (baseAddr << 21);
	VDP_DATA_L = p[0];
	VDP_DATA_L = p[1];
	VDP_DATA_L = p[2];
	VDP_DATA_L = p[3];
	VDP_DATA_L = p[4];
	VDP_DATA_L = p[5];
	VDP_DATA_L = p[6];
	VDP_DATA_L = p[7];
}

void video_upload_sprite(const uint32_t *data,uint32_t count) {
	while (count--) {
		VDP_DATA_L = data[0];
		VDP_DATA_L = data[1];
		VDP_DATA_L = data[2];
		VDP_DATA_L = data[3];
		VDP_DATA_L = data[4];
		VDP_DATA_L = data[5];
		VDP_DATA_L = data[6];
		VDP_DATA_L = data[7];
		data+=8;
	}
}

void video_set_palette_entry(uint8_t baseAddr,uint16_t pe) {
	md_assert(baseAddr < 64);
	VDP_CTRL_L = 0xC0000000 + (baseAddr << 17);
	VDP_DATA_W = pe;
}

uint8_t video_first_char;

void video_upload_bitmap_font(const uint8_t *bitmap,uint32_t count,uint8_t c0,uint8_t c1,uint8_t firstChar) {
	uint8_t a[2] = { c0, c1 };
	video_first_char = firstChar;
	bitmap += 8 * firstChar;
	count *= 8;
	do {
		uint8_t b = *bitmap++;
		uint32_t x = 0;
		for (int i=0; i<8; i++, b>>=1)
			x = (x << 4) | a[b&1];
		VDP_DATA_L = x;
	} while (--count);
}

void video_draw_string(uint16_t addr,uint16_t attrib,const char *s) {
	video_set_vram_write_addr(addr);
	while (*s)
		VDP_DATA_W = (*s++ - video_first_char) | attrib;
}
