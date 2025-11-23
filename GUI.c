#include "gui.h"
#include <stdlib.h>

// Color pair definitions
#define CP_TEXT 1
#define CP_HIGHLIGHT 2

void gui_init() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        // BIOS style: perhaps light green / white text on black background
        init_pair(CP_TEXT, COLOR_GREEN, COLOR_BLACK);
        // highlight maybe inverse or bright
        init_pair(CP_HIGHLIGHT, COLOR_BLACK, COLOR_GREEN);
    }
}

void gui_end() {
    endwin();
}

Gui *gui_create(const char **options, int n_options) {
    Gui *g = malloc(sizeof(Gui));
    g->options = options;
    g->n_options = n_options;
    g->selected = 0;

    // size the window to something reasonable
    int max_len = 0;
    for (int i = 0; i < n_options; i++) {
        int len = strlen(options[i]);
        if (len > max_len) max_len = len;
    }
    g->width = max_len + 4;
    g->height = n_options + 2;

    int startx = (COLS - g->width) / 2;
    int starty = (LINES - g->height) / 2;
    g->win = newwin(g->height, g->width, starty, startx);
    keypad(g->win, TRUE);

    return g;
}

void gui_destroy(Gui *g) {
    delwin(g->win);
    free(g);
}

void gui_draw(Gui *g) {
    werase(g->win);

    // Fill background with black color pair
    wbkgd(g->win, COLOR_PAIR(CP_TEXT));

    box(g->win, 0, 0);

    for (int i = 0; i < g->n_options; i++) {
        if (i == g->selected) {
            wattron(g->win, COLOR_PAIR(CP_HIGHLIGHT) | A_BOLD);
        } else {
            wattron(g->win, COLOR_PAIR(CP_TEXT));
        }

        // draw the option
        mvwprintw(g->win, i + 1, 2, "%s", g->options[i]);

        // turn off attributes
        wattroff(g->win, COLOR_PAIR(CP_HIGHLIGHT) | A_BOLD);
        wattroff(g->win, COLOR_PAIR(CP_TEXT));
    }

    wrefresh(g->win);
}

int gui_run(Gui *g) {
    int ch;
    while (1) {
        gui_draw(g);
        ch = wgetch(g->win);
        switch (ch) {
            case KEY_UP:
                g->selected--;
                if (g->selected < 0) g->selected = g->n_options - 1;
                break;
            case KEY_DOWN:
                g->selected++;
                if (g->selected >= g->n_options) g->selected = 0;
                break;
            case '\n':
            case KEY_ENTER:
                return g->selected;
            case 27: // ESC key
                return -1;
        }
    }
}
