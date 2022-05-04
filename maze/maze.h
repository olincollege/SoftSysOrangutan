void draw_maze();
void clrscr();
void cputsxy(unsigned int x, unsigned int y, char outString[255]);
int run_maze();
void end_setup();
int check_screen_size();
void update_sprite(int *x, int *y, int key);
int check_win(int *x, int *y);
void check_edges(int *x, int *y, int current_x, int current_y);