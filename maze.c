// Just trying out examples with ncurses for right now
#include <ncurses.h>			/* ncurses.h includes stdio.h */  
#include <string.h> 
#include <time.h>
#include <stdlib.h>
#include <time.h> 
#include "maze_array.h"
// #include "maze.h"
 
typedef struct{
	int y;
	int x;
} Point;

// int fill_maze(int screen_height, int screen_width, char str[3]);
void draw_maze(int screen_height, int screen_width);
void draw_horizantal_line(Point *starting, int end_x, char ch);
void draw_vertical_line(Point *starting, int end_y, char ch);
void draw_outline(int starty, int startx, char ch);
void clrscr();
void cputsxy(unsigned int x, unsigned int y, char outString[255]);



int key;
int main() {	
	clrscr();
	int screen_height, screen_width;
	clock_t t;
	time_t start, stop;
	getmaxyx(stdscr,screen_height,screen_width); // get the number of rows and columns 	
	// initial x and y positions of sprite
	int starty = screen_height/2 - 15;
	int startx = screen_width/2 - 33;
	int x = startx + 36;
	int y = starty + 2;
	draw_maze(starty, startx);
	cputsxy(x,y,"@");
	start = time(NULL);
	while ((key  = getch()) != 'q') {		
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
		if (mvinch(y, x) == '#'){
			x = current_x;
			y = current_y;
		}
		if (mvinch(y, x) == '*'){
			cputsxy(x,y,"@");
			stop = time(NULL);
			// endwin();
			endwin();
			
			printf ("You took %ld seconds to complete the maze!\n",stop - start);
			break;
			// return 0;
			
			
		}
		cputsxy(x,y,"@");
	
    }
    /* Turn echo and cursor back on */
    echo();
    curs_set(1);
    /* End program */
	endwin();
	return 0;
}

void draw_maze(int starty, int startx){
	// int starty = screen_height/2 - 15;
	// int startx = screen_width/2 - 33;
	for(int i = 0; i < 30; i++){
		for(int j = 0; j < 39; j++){
			if (maze_array[i][j] == 0){
				mvprintw(starty + i, startx + j*2 , "##");
			}
		}
	}
	mvprintw(starty + 1, startx + 32, "**");
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



