#include "GUI.h"
#include <stdlib.h>


void gui_init() 
{
    initscr();
    if (has_colors()) 
    {
        start_color();
		printw("Color Supported\n");
        // BIOS style: perhaps light green / white text on black background
        init_pair(CP_TEXT, COLOR_WHITE, COLOR_BLACK);			// Needs error handling
		attron(COLOR_PAIR(CP_TEXT));
        // highlight maybe inverse or bright
        init_pair(CP_SUBWINDOW, COLOR_WHITE, COLOR_BLUE);		// Needs error handling
    } else {
		printw("Color Unsupported\n");
	}
	cbreak();
    noecho();
    keypad(stdscr, TRUE);		// Starting keypad mode
    curs_set(0);
	refresh();
	napms(1500);
	//clear();
}

void create_window(WINDOW* newWindow, int height, int width, int ypos, int xpos)
{
	newWindow = newwin(height, width, ypos, xpos);
	wbkgd(newWindow, CP_TEXT);
	box(newWindow, 0, 0);
	wrefresh(newWindow);
}

