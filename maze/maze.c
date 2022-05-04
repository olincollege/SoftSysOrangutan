// Just trying out examples with ncurses for right now
#include <ncurses.h>			/* ncurses.h includes stdio.h */  
#include <string.h> 
#include <time.h>
#include <stdlib.h>
#include <time.h> 
#include "maze_array.h"
#include "maze.h"

int run_maze(){
	clrscr();
	int key;
	int screen_height, screen_width;
	time_t start, stop;
	getmaxyx(stdscr,screen_height,screen_width);
	int starty = 5;//screen_height/4;//screen_height/2 - 15;
	int startx = 5;//screen_width/4;//screen_width/2 - 33;
	int x = startx + 36;
	int y = starty + 2;
	int final_time;
	// draw_maze(starty, startx);
	cputsxy(x,y,"@");
	start = time(NULL);
	while ((key  = getch()) != 'q') {
		draw_maze(starty, startx);		
		/* Delete the character */		
		cputsxy(x,y," ");
		/* Handle keys */
		int current_x = x;
		int current_y = y;
		switch (key) { 
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
			final_time = stop - start;
			end_setup();
			return final_time;				
		}
		cputsxy(x,y,"@");
	
    }
    /* Turn echo and cursor back on */
	end_setup();
	return -1;
}

void end_setup(){
	echo();
    curs_set(1);
    /* End program */
	endwin();
}

void draw_maze(int starty, int startx){
	for(int i = 0; i < 30; i++){
		for(int j = 0; j < 39; j++){
			if (maze_array[i][j] == 0){
				mvaddstr(starty + i, startx + j*2 , "##");
			}
		}
	}
	mvprintw(starty + 1, startx + 32, "**");
}


void cputsxy(unsigned int x, unsigned int y, char outString[255]){
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



