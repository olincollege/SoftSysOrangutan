
#include <ncurses.h>			/* ncurses.h includes stdio.h */  
#include <string.h> 
#include <time.h>
#include <stdlib.h>
#include <time.h> 
#include "maze_array.h"
#include "maze.h"

int run_maze(){
	/* Runs the main loop of the maze. Returns an int representing the time 
	taken to complete the maze or -1 if the player quits ahead of time.*/
	clrscr();
	int key;
	int screen_height, screen_width;
	time_t start, stop;
	int x = 36; int y = 2;
	int current_x; int current_y;
	int end_flag = 1;

	cputsxy(x,y,"@");
	start = time(NULL);
	while (end_flag) {
		// Draw the maze
		if (check_screen_size())
			draw_maze();

		// Receive key press
		key = getch();	

		// Delete and store current position
		cputsxy(x,y," ");
		current_x = x;
		current_y = y;

		// Update sprite position
		update_sprite(&x, &y, key);
		// Check for edges
		check_edges(&x, &y, current_x, current_y);
		
		// check win
		if (check_win(&x, &y)){
			cputsxy(x,y,"@");
			stop = time(NULL);
			int final_time = stop - start;
			end_setup();
			return final_time;				
		}
		cputsxy(x,y,"@");

		// check for quitting
		if(key == 'q')
			end_flag = 0;	
    }
    /* Turn echo and cursor back on */
	end_setup();
	return -1;
}

int check_screen_size(){
	/* Checks to see if the screen size is large enough
	   to print the maze. Returns 1 if it or 0 if its not.*/
	
	int screen_height; int screen_width;
	// Getting the maximum screen size
	getmaxyx(stdscr,screen_height,screen_width);
	// Printing a message if its not large enough
	if((screen_width < 80)||(screen_height < 30)){
		erase();
		mvprintw(1, 2 , "Increase window size");
		return 0;
	} else
		return 1;
}

void check_edges(int *x, int *y, int current_x, int current_y){
	/* Checks to see if the player position is colliding with the walls and 
	   restores the player position to its previous position if it was.*/	
	if (mvinch(*y, *x) == '#'){
	*x = current_x;
	*y = current_y;
	}
}

int check_win(int *x, int *y){
	/* Checks to see if the player has finished the maze and returns 1 if it has 
	   and 0 otherwise.*/
	if(((*x == 32) || (*x == 33)) && (*y <= 1)){
		return 1;
	} else {
		return 0;
	}
}

void end_setup(){
	/* Restores the terminal window to what it was before starting the game*/
	echo();
    curs_set(1);
    /* End program */
	endwin();
}

void update_sprite(int *x, int *y, int key){
	/* Updates the position of the sprite based on keypresses*/	
	switch (key) { 
			case 'w': 
				(*y)--; 
				break; 
			case 'a': 
				(*x)--; 
				break; 
			case 's': 
				(*y)++; 
				break; 
			case 'd': 
				(*x)++; 
				break; 
	}
}

void draw_maze(){
	/* Draws the maze and finish line*/	
	for(int i = 0; i < 30; i++){
		for(int j = 0; j < 39; j++){
			if (maze_array[i][j] == 0){
				mvaddstr(i, j*2 , "##");
			}
		}
	}
	mvaddstr(0, 30, "Finish");
	mvaddstr(1, 32, "/");
	mvaddstr(1, 33, "\\");
	move(0, 0);
}


void cputsxy(unsigned int x, unsigned int y, char outString[255]){
	/* Prints a character at a certain coordinate.*/	
    mvprintw(y,x, outString);
}

void clrscr() {
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



