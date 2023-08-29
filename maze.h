extern void maze_init();
extern void maze_draw(int,int);

extern void maze_new_right_column(uint32_t off_x,uint32_t off_y);
extern void maze_new_left_column(uint32_t off_x,uint32_t off_y);

// Must be a multiple of eight
#define MAZE_SIZE 128
