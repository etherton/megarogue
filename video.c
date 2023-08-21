#include "md_api.h"

void video_init() {
	VDP_CTRL_W = 0x8004; // No HBI, no HV latch
	VDP_CTRL_W = 0x8134; // No display, VBI, DMA OK, V28
	VDP_CTRL_W = 0x8230; // Plane A: $C000
	VDP_CTRL_W = 0x8407; // Plane B: $E000
	VDP_CTRL_W = 0x8578; // Sprites: $F000
	VDP_CTRL_W = 0x8700; // BG color: palette 0, index 0
	VDP_CTRL_W = 0x8B00; // No IRQ2, full scrolling
	VDP_CTRL_W = 0x8C81; // H40, no S/H, no interlace
	VDP_CTRL_W = 0x8D3E; // Hscroll: $F800
	VDP_CTRL_W = 0x8F02; // Autoincrement: 2 bytes
	VDP_CTRL_W = 0x9001; // Tilemap size: 64x32
	VDP_CTRL_W = 0x9100; // Hide window plane
	VDP_CTRL_W = 0x9200; // Hide window plane
}

void video_enable() {
	VDP_CTRL_W = 0x8174;
}

void video_load_palette(uint8_t baseAddr,const uint16_t *palette,uint8_t count) {
	md_assert(baseAddr < 64);
	VDP_CTRL_L = 0xC0000000 + (baseAddr << 16);
	for (;count>=2; count-=2,palette+=2)
		VDP_DATA_L = *(uint32_t*) palette;
	VDP_DATA_W = *palette;
}

void video_set_palette_entry(uint8_t baseAddr,uint16_t pe) {
	md_assert(baseAddr < 64);
	VDP_CTRL_L = 0xC0000000 + (baseAddr << 17);
	VDP_DATA_W = pe;
}

void video_upload_bitmap_font(const uint8_t *bitmap,uint32_t count,uint8_t c0,uint8_t c1) {
	uint8_t a[2] = { c0, c1 };
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
		VDP_DATA_W = *s++ | attrib;
}
