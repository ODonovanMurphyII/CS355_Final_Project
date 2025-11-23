#ifndef GUI_H
#define GUI_H

#include <ncurses.h>

// A simple GUI state
typedef struct {
    WINDOW *win;
    int width;
    int height;
    const char **options;
    int n_options;
    int selected;
} Gui;

Gui *gui_create(const char **options, int n_options);
void gui_destroy(Gui *g);

void gui_init();
void gui_end();

int gui_run(Gui *g);

void gui_draw(Gui *g);

#endif // GUI_H
