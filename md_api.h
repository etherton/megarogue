// #include <stdint.h>
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#ifdef NDEBUG
#define md_assert(x)
#else
#define md_assert(x)	do { if (!(x)) asm(".short 0x4E43"); } while (0) // TRAP 3
#endif

#define REG_VERSION_B (*(volatile uint8_t*)0xA10001)
#define REG_VERSION_TMSS	0x0F	// If nonzero, need to write 'SEGA' to REG_TMSS
#define REG_VERSION_NTSC	0x00
#define REG_VERSION_PAL		0x40
#define REG_VERSION_DOMESTIC	0x00
#define REG_VERSION_OVERSEAS	0x80

#define REG_TMSS_L		(*(uint32_t*)0xA14000)

#define REG_DATA_B(x) (*(volatile uint8_t*)(0xA10003+(((x)&1)<<1)))
#define REG_DATA1_B (*(volatile uint8_t*)0xA10003)
#define REG_DATA2_B (*(volatile uint8_t*)0xA10005)
#define REG_DATA3_B (*(volatile uint8_t*)0xA10007)
#define REG_CTRL_B(x) (*(volatile uint8_t*)(0xA10009+(((x)&1)<<1)))
#define REG_CTRL1_B (*(volatile uint8_t*)0xA10009)
#define REG_CTRL2_B (*(volatile uint8_t*)0xA1000B)
#define REG_CTRL3_B (*(volatile uint8_t*)0xA1000D)

#define VDP_DATA_B (*(volatile uint8_t*)0xC00000)
#define VDP_DATA_W (*(volatile uint16_t*)0xC00000)
#define VDP_DATA_L (*(volatile uint32_t*)0xC00000)
#define VDP_CTRL_W (*(volatile uint16_t*)0xC00004)
#define VDP_CTRL_L (*(volatile uint32_t*)0xC00004)

extern void start();
extern uint16_t joypad_read(int which);
#define JOYPAD_UP 	0x0001
#define JOYPAD_DOWN	0x0002
#define JOYPAD_LEFT	0x0004
#define JOYPAD_RIGHT	0x0008
#define JOYPAD_B	0x0010
#define JOYPAD_C	0x0020
#define JOYPAD_A	0x0040
#define JOYPAD_START	0x0080
#define JOYPAD_Z	0x0100
#define JOYPAD_Y	0x0200
#define JOYPAD_X	0x0400
#define JOYPAD_MODE	0x0800
#define JOYPAD_6	0x8000

extern void video_init();
extern void video_enable();
extern void video_load_palette(uint8_t base,const uint16_t* values,uint8_t count);
extern void video_set_palette_entry(uint8_t base,uint16_t entry);
extern void video_upload_bitmap_font(const uint8_t*,uint32_t count,uint8_t c0,uint8_t c1);

inline void video_set_vram_write_addr(uint16_t addr) {
	VDP_CTRL_L = 0x40000000 | ((addr & 0x3FFF) << 16) | (addr >> 14);
}
extern void video_draw_string(uint16_t addr,uint16_t attributes,const char *s);
// These assume 64 tiles wide
inline uint16_t video_plane_a_addr(uint8_t x,uint8_t y) {
	return y * 128 + x * 2 + 0xC000;
}
inline uint16_t video_plane_b_addr(uint8_t x,uint8_t y) {
	return y * 128 + x * 2 + 0xE000;
}

struct nametable_t {
	uint16_t priority:1, palette:2, vFlip:1, hFlip:1, tileIndex:11;
};

union sprite_t {
	struct {
		uint16_t vPos;
		uint16_t unused1:4, hSize: 2, vSize: 2, link: 8;
		struct nametable_t def;
		uint16_t hPos;
	} asStruct;
	uint16_t asShort[4];
	uint32_t asLong[2];
};
