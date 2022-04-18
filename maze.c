// Just trying out examples with ncurses for right now
#include <ncurses.h>			/* ncurses.h includes stdio.h */  
#include <string.h> 
#include <time.h>
#include <stdlib.h>
// #include "maze.h"
 
typedef struct{
	int y;
	int x;
} Point;

void draw_maze(int screen_height, int screen_width, char ch);
void draw_horizantal_line(Point *starting, int end_x, char ch);
void draw_vertical_line(Point *starting, int end_y, char ch);
void draw_outline(int screen_height, int screen_width, char ch);
void clrscr();
void cputsxy(unsigned int x, unsigned int y, char outString[255]);



int key;
int main()
{
	
	clrscr();
	getch();
	int screen_height, screen_width;
	char ch = '*';
	getmaxyx(stdscr,screen_height,screen_width); // get the number of rows and columns 	
	// maze_array

	// initial x and y positions
	int x = screen_width / 4;
	int y = screen_height / 4 + 4 * screen_height / 10;
		
	while ((key  = getch()) != 'q')
    {				
		draw_maze(screen_height, screen_width, ch);
		draw_outline(screen_height, screen_width, ch);

		/* Delete the character */
		cputsxy(x,y," ");
		/* Handle keys */
		int current_x = x;
		int current_y = y;
		switch (key) 
		{ 
			case 'w': 
				y--; 
				break; 
			case 'a': 
				x--; 
				break; 
			case 's': 
				y++; 
				break; 
			case 'd': 
				x++; 
				break; 
			default: 
				break; 
		}

		// checking for edges of the maze
		if (mvinch(y, x) == '*'){
			x = current_x;
			y = current_y;
		}
		cputsxy(x,y,"@");
		/* Refresh the terminal */
		refresh();
	
    }
    /* Turn echo and cursor back on */
    echo();
    curs_set(1);
    /* End program */
	endwin();
	return 0;
}

void draw_horizantal_line(Point *starting, int end_x, char ch){
	for (int i = starting->x; i <= end_x; i++){
		mvaddch(starting->y, i , ch);
	}
}

void draw_vertical_line(Point *starting, int end_y, char ch){
	for (int i = starting->y; i <= end_y; i++){
		mvaddch(i, starting->x , ch);
	}
}

void draw_maze(int screen_height, int screen_width, char ch){
	int starty = screen_height/4;
	int startx = screen_width/4;
	int height = 3 * screen_height/4 - screen_height/4;
	int width = 3 * screen_width/4 - screen_width/4;
	Point one = {starty + height/4, startx};
	draw_horizantal_line(&one, startx + 3*width/4, ch);

	Point two = {starty + height/2, startx + width/4};
	draw_horizantal_line(&two, startx + width, ch);

	Point three = {starty + 3 * height/4, startx};
	draw_horizantal_line(&three, startx + 3*width/4, ch);

	draw_vertical_line(&one,starty + 3 * height/4, ch);

}

void draw_outline(int screen_height, int screen_width, char ch){	
	Point tl = {screen_height/4, screen_width/4};
	Point tr = {screen_height/4, 3 * screen_width/4};	
	Point bl = {3 * screen_height/4, screen_width/4};
	Point br = {3 * screen_height/4,3 * screen_width/4};
	draw_horizantal_line(&tl, (&tr)->x, ch);
	draw_horizantal_line(&bl, (&br)->x, ch);
	draw_vertical_line(&tr, (&br)->y, ch);
}

void cputsxy(unsigned int x, unsigned int y, char outString[255])
{
    mvprintw(y,x, outString);
}

void clrscr() 
{
    /* Initialise the screen */
    initscr();
    erase();
    noecho();
    raw();
    move(0, 0);
    /* Cursor off */
    curs_set(0);
    refresh();	
}



