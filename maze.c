#include "md_api.h"
#include "md_math.h"
#include "maze.h"

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


// Must be a multiple of 8
uint8_t maze[MAZE_SIZE][MAZE_SIZE/8];
const uint8_t left[] =  { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
const uint8_t right[] = { 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };
const uint8_t bit[] =   { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

static void draw_row(int row,int l,int r) {
	assert(row>=0&&row<MAZE_SIZE);
	assert(l<=r);
	assert(l>=0);
	assert(r<MAZE_SIZE);
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

static void draw_col(int col,int t,int b) {
	assert(col>=0&&col<MAZE_SIZE);
	assert(t<=b);
	assert(t>=0);
	assert(b<MAZE_SIZE);
	while (t<=b)
		maze[t++][col>>3] |= bit[col&7];
}

static void unset(int row,int col) {
	maze[row][col>>3] &= ~bit[col&7];
}

#if 0
void print_maze() {
	char c = '*', s = ' ';
	for (int row=0; row<MAZE_SIZE; row++) {
		for (int col=0; col<MAZE_SIZE/8; col++) {
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


uint32_t seed;

static uint32_t Random(uint32_t range) {
	seed = mul32(seed,1103515245U) + 12345;
	if ((range << 15) < seed)
		return modulo((uint16_t)seed,range);
	else
		return modulo(seed,range);
}

/* X .. X
   ^    ^
   |    stop
   start
   If we want any remaining randomness, they cannot be any closer than 5 */
static void make_maze(int axis,int start,int stop,int otherStart,int otherStop) {
	assert(start<stop);
	assert(otherStart<otherStop);
	if (stop-start <= 5)
		return;
	int split = start + 2 + Random(stop - start - 3);
	if (axis & 1) {
		draw_row(split,otherStart,otherStop);
		unset(split,(otherStart + otherStop)>>1);
	}
	else {
		draw_col(split,otherStart,otherStop);
		unset((otherStart + otherStop)>>1,split);
	}
	if (split - start  > 2 * (otherStop - otherStart))
		make_maze(axis,start,split,otherStart,otherStop);
	else
		make_maze(axis ^ 1,otherStart,otherStop,start,split);
	if (stop - split > 2 * (otherStop - otherStart))
		make_maze(axis,split,stop,otherStart,otherStop);
	else
		make_maze(axis ^ 1,otherStart,otherStop,split,stop);
}

static uint16_t maze_base_tile;
static const uint8_t *maze_palettes;

void maze_init(int base_tile,const uint8_t *palettes) {
	maze_base_tile = base_tile;
	maze_palettes = palettes;

	draw_row(0,0,MAZE_SIZE-1);
	draw_row(MAZE_SIZE-1,0,MAZE_SIZE-1);
	draw_col(0,0,MAZE_SIZE-1);
	draw_col(MAZE_SIZE-1,0,MAZE_SIZE-1);
	make_maze(Random(2),0,MAZE_SIZE-1,0,MAZE_SIZE-1);
}

static inline _Bool test(int r,int c) {
	return r>=0&r<MAZE_SIZE&&c>=0&&c<MAZE_SIZE?(maze[r][c>>3] & bit[c&7]) != 0 : 0;
}

int maze_get_tile(int row,int col) {
	if (!test(row,col))
		return 4;
	int bit_u = test(row-1,col)? OCC_U : 0;
	int bit_d = test(row+1,col)? OCC_D : 0;
	int bit_l = test(row,col-1)? OCC_L : 0;
	int bit_r = test(row,col+1)? OCC_R : 0;
	uint16_t tile = dirMap[bit_u | bit_d | bit_l | bit_r];
	return tile;
}

void maze_draw(int off_x,int off_y) {
	for (int row=0; row<11; row++) {
		for (int col=0; col<22; col++) {
			int tile = maze_get_tile(row,col);
			uint16_t attr = maze_palettes[tile] << 13;
			tile = maze_base_tile + tile * 9;
			video_set_vram_write_addr(video_plane_b_addr(col*3,row*3));
			VDP_DATA_W=(tile+0) | attr;
			if (col != 21) {
				VDP_DATA_W=(tile+3) | attr;
				VDP_DATA_W=(tile+6) | attr;
			}
			video_set_vram_write_addr(video_plane_b_addr(col*3,row*3+1));
			VDP_DATA_W=(tile+1) | attr;
			if (col != 21) {
				VDP_DATA_W=(tile+4) | attr;
				VDP_DATA_W=(tile+7) | attr;
			}
			if (row!=10) {
				video_set_vram_write_addr(video_plane_b_addr(col*3,row*3+2));
				VDP_DATA_W=(tile+2) | attr;
				if (col != 21) {
					VDP_DATA_W=(tile+5) | attr;
					VDP_DATA_W=(tile+8) | attr;
				}
			}
		}
	}
}

void maze_new_column(uint32_t off_x,uint32_t off_y,uint32_t vid_col) {
	VDP_CTRL_W = 0x8F00 | (video_plane_width<<1);	// Autoincrement = one canvas line
	uint32_t temp_x = div_mod(off_x,3);
	uint32_t temp_y = div_mod(off_y,3);
	uint16_t tile_x = (uint16_t)temp_x, subtile_x = (uint16_t)(temp_x>>16) * 3;
	uint16_t tile_y = (uint16_t)temp_y, subtile_y = (uint16_t)(temp_y>>16);
	video_set_vram_write_addr(video_plane_b_addr(vid_col,off_y & video_plane_height_mask));
	uint16_t tile = maze_get_tile(tile_y,tile_x);
	uint16_t attr = maze_palettes[tile] << 13;
	tile = maze_base_tile + tile*9 + subtile_y + subtile_x;
	uint16_t r=video_plane_height; 
	do {
		VDP_DATA_W = tile | attr;
		if (++subtile_y==3) {
			++tile_y;
			subtile_y=0;
			tile = maze_get_tile(tile_y,tile_x);
			attr = maze_palettes[tile] << 13;
			tile = maze_base_tile + tile*9 + subtile_x;
		}
		else
			++tile;
		if ((++off_y & video_plane_height_mask) == 0)
			video_set_vram_write_addr(video_plane_b_addr(vid_col,0));
	} while (--r);
	VDP_CTRL_W = 0x8F02;	// Autoincrement = 2;
}

void maze_new_right_column(uint32_t off_x,uint32_t off_y) {
	maze_new_column(off_x+video_plane_width_mask,off_y,(off_x+video_plane_width_mask)&video_plane_width_mask);
}

void maze_new_left_column(uint32_t off_x,uint32_t off_y) {
	maze_new_column(off_x,off_y,off_x&video_plane_width_mask);
}

void maze_new_row(uint32_t off_x,uint32_t off_y,uint32_t vid_row) {
	uint32_t temp_x = div_mod(off_x,3);
	uint32_t temp_y = div_mod(off_y,3);
	uint16_t tile_x = (uint16_t)temp_x, subtile_x = (uint16_t)(temp_x>>16) * 3;
	uint16_t tile_y = (uint16_t)temp_y, subtile_y = (uint16_t)(temp_y>>16);
	video_set_vram_write_addr(video_plane_b_addr(off_x & video_plane_width_mask,vid_row));
	uint16_t tile = maze_get_tile(tile_y,tile_x);
	uint16_t attr = maze_palettes[tile] << 13;
	tile = maze_base_tile + tile*9 + subtile_y + subtile_x;
	uint16_t c=video_plane_width;
	do {
		VDP_DATA_W = tile | attr;
		if ((subtile_x+=3)==9) {
			++tile_x;
			subtile_x=0;
			tile = maze_get_tile(tile_y,tile_x);
			attr = maze_palettes[tile] << 13;
			tile = maze_base_tile + tile*9 + subtile_y;
		}
		else
			tile+=3;
		if ((++off_x & video_plane_width_mask) == 0)
			video_set_vram_write_addr(video_plane_b_addr(0,vid_row));
	} while (--c);
}

void maze_new_bottom_row(uint32_t off_x,uint32_t off_y) {
	maze_new_row(off_x,off_y+video_plane_height_mask,(off_y+video_plane_height_mask)&video_plane_height_mask);
}

void maze_new_top_row(uint32_t off_x,uint32_t off_y) {
	maze_new_row(off_x,off_y,off_y&video_plane_height_mask);
}

/*
	scrolling algorithm:

	abcdefgh

	As soon has 'a' has scrolled completely off the screen, replace it with 'i'.
	Conceptually, this is (offset - 1) masked to canvas width.

	Consider a simpler case diagonally: (canvas is 4x4, screen is 3x3)
	(capital letters indicate the portion visible on screen)

	ABCd            eBCD
	FGHi     Pan    jGHI
	KLMn    right:  oLMN
	pqrs            tqrs

	Pan down:     Pan digonally:

	uvwx		uvwx
	FGHi		jGHI
	KLMn		oLMN
	PQRs		tQRS
*/
