extern void maze_init(int base_tile,const uint8_t *palettes);
extern void maze_draw();

extern void maze_new_right_column(uint32_t off_x,uint32_t off_y);
extern void maze_new_left_column(uint32_t off_x,uint32_t off_y);
extern void maze_new_bottom_row(uint32_t off_x,uint32_t off_y);
extern void maze_new_top_row(uint32_t off_x,uint32_t off_y);

// Must be a multiple of eight
#define MAZE_SIZE 64
