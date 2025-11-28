#ifndef GUI_H
#define GUI_H
#define SUB_WINDOW_WIDTH COLS/2
#define SUB_WINDOW_HEIGHT LINES/2
#define CP_TEXT 1
#define CP_HIGHLIGHT 2
#define CP_SUBWINDOW 3
#include <ncurses.h>

void gui_init();
void create_window(WINDOW* newWindow, int height, int width, int ypos, int xpos);


#endif // GUI_H
