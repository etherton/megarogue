#include "md_api.h"
#include "md_math.h"

#define assert md_assert

/*

00 1x1 solid
01 1x1 broken
02 1x1 really broken
03 floor 1
04 floor 2
05 floor 3
06 floor 4
07 stairs up
08 stairs down

09 1x1 wall
10 horizontal left end
11 horizontal middle
12 horizontal right end
13 vertical top end
14 vertical middle
15 vertical bottom end

16 upper left corner
17 upper right corner
18 lower left corner
19 lower right corner

20 4 way intersection
21 3 way T (not up)
22 3 way T (not right)
23 3 way T (not left)
24 3 way T (not down)

26 broken vertical
27 broken horizontal

*/

#define OCC_U 8
#define OCC_D 4
#define OCC_L 2
#define OCC_R 1
// Given occupancy bit mask as above (relative to current wall), return appropriate tile
const uint8_t dirMap[16] = { 9, 10, 12, 11, 13, 16, 17, 21, 15, 18, 19, 24, 14, 23, 22, 20 };

/*
type[0b0000] = 9; // 1x1 wall
type[0b0001] = 10; // horizontal left end
type[0b0010] = 12; // horizontal right end
type[0b0011] = 11; // horizontal middle
type[0b0100] = 13; // vertical top end
type[0b0101] = 16; // upper left corner
type[0b0110] = 17; // upper right corner
type[0b0111] = 21; // 3 way T (not up)
type[0b1000] = 15; // vertical bottom end
type[0b1001] = 18; // lower left corner
type[0b1010] = 19; // lower right corner
type[0b1011] = 24; // 3 way T (not down)
type[0b1100] = 14; // vertical middle
type[0b1101] = 23; // 3 way T (not left)
type[0b1110] = 22; // 3 way T (not right)
type[0b1111] = 20; // 4 way intersection
*/


#define SIZ 128 
// const uint32_t SIZ = 128;
uint8_t maze[SIZ][SIZ/8];
const uint8_t left[] =  { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
const uint8_t right[] = { 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };

void draw_row(int row,int l,int r) {
	assert(row>=0&&row<SIZ);
	assert(l<=r);
	assert(l>=0);
	assert(r<SIZ);
	if ((l>>3)==(r>>3))
		maze[row][l>>3] |= left[l&7] & right[r&7];
	else {
		maze[row][l>>3] |= left[l&7];
		maze[row][r>>3] |= right[r&7];
		l>>=3; r>>=3;
		while (l+1 != r)
			maze[row][++l]=0xff;
	}
}

void draw_col(int col,int t,int b) {
	assert(col>=0&&col<SIZ);
	assert(t<=b);
	assert(t>=0);
	assert(b<SIZ);
	while (t<=b)
		maze[t++][col>>3] |= left[col&7] & right[col&7];
}

#if 0
void print_maze() {
	char c = '*', s = ' ';
	for (int row=0; row<SIZ; row++) {
		for (int col=0; col<SIZ/8; col++) {
			putchar(maze[row][col] & 0x80?c:s);
			putchar(maze[row][col] & 0x40?c:s);
			putchar(maze[row][col] & 0x20?c:s);
			putchar(maze[row][col] & 0x10?c:s);
			putchar(maze[row][col] & 0x08?c:s);
			putchar(maze[row][col] & 0x04?c:s);
			putchar(maze[row][col] & 0x02?c:s);
			putchar(maze[row][col] & 0x01?c:s);
		}
		putchar('\n');
	}
}
#endif


uint32_t mul32(uint32_t ab,uint32_t cd) {
	// AB * CD = (AC << 32) + ((B*C + A*D)<<16) + B*D
	return ((mul16((uint16_t)ab,(uint16_t)(cd>>16)) + mul16((uint16_t)(ab>>16),(uint16_t)cd)) << 16) + mul16((uint16_t)ab,(uint16_t)cd);
}

uint32_t seed;

static inline int Random(uint16_t r) {
	if (!seed) seed=1;
	seed = mul32(seed,1103515245U) + 12345;
	return modulo(seed,r);
	// return arc4random() % r;
}

inline int PACK(int a,int b,int c,int d,int e) {
	return (a << 28) | (b << 21) | (c << 14) | (d << 7) | e;
}

void make_maze(int pack) {
	int axis = pack >> 28;
	int start = (pack >> 21) & 127;
	int stop = (pack >> 14) & 127;
	int otherStart = (pack >> 7) & 127;
	int otherStop = pack & 127;
	assert(start<stop);
	assert(otherStart<otherStop);
	if (stop-start < 10)
		return;
	if (otherStop-otherStart < 10)
		return;
	int split = start + 3 + Random(stop - start - 7);
	if (axis)
		draw_row(split,otherStart,otherStop);
	else
		draw_col(split,otherStart,otherStop);
	if (split - start  > 2 * (otherStop - otherStart))
		make_maze(PACK(axis,start,split,otherStart,otherStop));
	else
		make_maze(PACK(!axis,otherStart,otherStop,start,split));
	if (stop - split > 2 * (otherStop - otherStart))
		make_maze(PACK(axis,split,stop,otherStart,otherStop));
	else
		make_maze(PACK(!axis,otherStart,otherStop,split,stop));
}

void init_maze() {
	draw_row(1,1,SIZ-2);
	draw_row(SIZ-2,1,SIZ-2);
	draw_col(1,1,SIZ-2);
	draw_col(SIZ-2,1,SIZ-2);
	// make_maze(PACK(Random(2),1,SIZ-2,1,SIZ-2));
}

void draw_maze(int off_x,int off_y) {
	for (int row=1; row<=9; row++) {
		for (int col=1; col<=14; col++) {
			int bit_u = (maze[row-1][(col+0)>>3] & (128>>((col+0)&7))) != 0? OCC_U : 0;;
			int bit_d = (maze[row+1][(col+0)>>3] & (128>>((col+0)&7))) != 0? OCC_D : 0;;
			int bit_l = (maze[row][(col-1)>>3] & (128>>((col-1)&7))) != 0? OCC_L : 0;;
			int bit_r = (maze[row][(col+1)>>3] & (128>>((col+1)&7))) != 0? OCC_R : 0;;
			uint16_t tile = dirMap[bit_u | bit_d | bit_l | bit_r] * 9 + 512;
			if (!(maze[row][col>>3] & (128>>(col&7))))
				tile = 6 * 9 + 512;
			video_set_vram_write_addr(video_plane_b_addr((col-1)*3,(row-1)*3));
			VDP_DATA_W=(tile+0) | NT_PALETTE_3;
			VDP_DATA_W=(tile+3) | NT_PALETTE_3;
			VDP_DATA_W=(tile+6) | NT_PALETTE_3;
			video_set_vram_write_addr(video_plane_b_addr((col-1)*3,(row-1)*3+1));
			VDP_DATA_W=(tile+1) | NT_PALETTE_3;
			VDP_DATA_W=(tile+4) | NT_PALETTE_3;
			VDP_DATA_W=(tile+7) | NT_PALETTE_3;
			video_set_vram_write_addr(video_plane_b_addr((col-1)*3,(row-1)*3+2));
			VDP_DATA_W=(tile+2) | NT_PALETTE_3;
			VDP_DATA_W=(tile+5) | NT_PALETTE_3;
			VDP_DATA_W=(tile+8) | NT_PALETTE_3;
		}
	}
}
