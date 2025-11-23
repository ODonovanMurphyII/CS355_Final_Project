#include "gui.h"
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Color pair IDs */
#define P_MAIN
#define P_HEADER
#define P_FOOTER
#define P_POPUP
#define P_HIGHLIGHT

static int left_width = 40;
static int right_width = 30;

void gui_init() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(P_MAIN, COLOR_WHITE, COLOR_BLUE);
        init_pair(P_HEADER, COLOR_YELLOW, COLOR_BLUE);
        init_pair(P_FOOTER, COLOR_BLACK, COLOR_CYAN);
        init_pair(P_POPUP, COLOR_WHITE, COLOR_CYAN);
        init_pair(P_HIGHLIGHT, COLOR_BLACK, COLOR_YELLOW);
    }
    bkgd(COLOR_PAIR(P_MAIN));
    clear();
    refresh();
}

void gui_shutdown() {
    endwin();
}

void gui_draw_frame() {
    attron(COLOR_PAIR(P_MAIN));
    clear();
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(P_MAIN));
}

void gui_draw_header(const char *title) {
    attron(COLOR_PAIR(P_HEADER) | A_BOLD);
    mvhline(1, 1, ' ', COLS - 2);
    mvprintw(1, 3, "%s", title);
    attroff(COLOR_PAIR(P_HEADER) | A_BOLD);
}

void gui_draw_footer(const char *legend) {
    int y = LINES - 2;
    attron(COLOR_PAIR(P_FOOTER) | A_BOLD);
    mvhline(y, 1, ' ', COLS - 2);
    mvprintw(y, 3, "%s", legend);
    attroff(COLOR_PAIR(P_FOOTER) | A_BOLD);
}

/* left pane file list */
void gui_draw_file_list(file_info *head, int selected_index) {
    int starty = 3;
    int startx = 2;
    int height = LINES - 6; /* leave space for header/footer */
    int width = left_width - 3;

    WINDOW *left = newwin(height, left_width, starty, startx);
    wbkgd(left, COLOR_PAIR(P_MAIN));
    box(left, 0, 0);
    mvwprintw(left, 0, 2, " Files ");

    int row = 1;
    int idx = 0;
    file_info *cur = head;
    while (cur && row < height - 1) {
        if (idx == selected_index) {
            wattron(left, COLOR_PAIR(P_HIGHLIGHT) | A_BOLD);
            mvwprintw(left, row, 2, "> %.*s", width - 4, cur->filename);
            wattroff(left, COLOR_PAIR(P_HIGHLIGHT) | A_BOLD);
        } else {
            wattron(left, COLOR_PAIR(P_MAIN));
            mvwprintw(left, row, 2, "  %.*s", width - 4, cur->filename);
            wattroff(left, COLOR_PAIR(P_MAIN));
        }
        idx++;
        row++;
        cur = cur->next;
    }

    /* scrollbar indicator if needed */
    if (idx > (height - 2)) {
        mvwprintw(left, height - 1, 2, "... (%d files)", idx);
    }

    wrefresh(left);
    delwin(left);
}

/* right pane file info */
void gui_draw_file_info(file_info *selected) {
    int starty = 3;
    int startx = left_width + 3;
    int height = LINES - 6;
    int width = right_width;

    WINDOW *right = newwin(height, width, starty, startx);
    wbkgd(right, COLOR_PAIR(P_MAIN));
    box(right, 0, 0);
    mvwprintw(right, 0, 2, " Info ");

    if (!selected) {
        mvwprintw(right, 2, 2, "No file selected");
    } else {
        mvwprintw(right, 2, 2, "Name: %s", selected->filename);
        mvwprintw(right, 3, 2, "Location: %s", selected->fileLocation);
        mvwprintw(right, 4, 2, "UID: %d  GID: %d", selected->userID, selected->groupID);
        mvwprintw(right, 5, 2, "Mode: %o", selected->mode);

        /* try small preview */
        char full[MAX_PATH_LEN + MAX_FILENAME_LEN + 4];
        snprintf(full, sizeof(full), "%s/%s", selected->fileLocation, selected->filename);
        FILE *f = fopen(full, "rb");
        if (f) {
            unsigned char buf[129];
            size_t n = fread(buf, 1, 128, f);
            fclose(f);
            mvwprintw(right, 7, 2, "Preview (first %zu bytes):", n);
            int line = 8;
            int col = 2;
            for (size_t i = 0; i < n && line < height - 1; ++i) {
                unsigned char c = buf[i];
                if (isprint(c)) {
                    mvwaddch(right, line, col, c);
                } else {
                    mvwaddch(right, line, col, '.');
                }
                col++;
                if (col >= width - 2) { col = 2; line++; }
            }
        } else {
            mvwprintw(right, 7, 2, "Unable to open file for preview.");
        }
    }

    wrefresh(right);
    delwin(right);
}

/* popup menu centered */
int gui_popup_menu(const char *title, const char *options[], int count) {
    int w = 40;
    int h = count + 6;
    if (h < 8) h = 8;
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    WINDOW *pop = newwin(h, w, y, x);
    keypad(pop, TRUE);
    wbkgd(pop, COLOR_PAIR(P_POPUP));
    box(pop, 0, 0);
    wattron(pop, A_BOLD | COLOR_PAIR(P_HEADER));
    mvwprintw(pop, 1, 2, " %s ", title);
    wattroff(pop, A_BOLD | COLOR_PAIR(P_HEADER));

    int sel = 0;
    int ch;
    while (1) {
        for (int i = 0; i < count; ++i) {
            if (i == sel) {
                wattron(pop, A_REVERSE);
                mvwprintw(pop, 3 + i, 3, "%s", options[i]);
                wattroff(pop, A_REVERSE);
            } else {
                mvwprintw(pop, 3 + i, 3, "%s", options[i]);
            }
        }
        mvwprintw(pop, h - 2, 2, "↑/↓ to move  Enter to select  ESC to cancel");
        wrefresh(pop);

        ch = wgetch(pop);
        if (ch == KEY_UP) {
            if (sel > 0) sel--;
        } else if (ch == KEY_DOWN) {
            if (sel < count - 1) sel++;
        } else if (ch == '\n' || ch == KEY_ENTER) {
            delwin(pop);
            return sel;
        } else if (ch == 27) { /* ESC */
            delwin(pop);
            return -1;
        }
    }
}

/* password prompt: returns 0 on success, -1 on ESC */
int gui_prompt_password(char *out, size_t maxlen) {
    int w = 60, h = 7;
    int y = (LINES - h) / 2, x = (COLS - w) / 2;
    WINDOW *pw = newwin(h, w, y, x);
    keypad(pw, TRUE);
    wbkgd(pw, COLOR_PAIR(P_POPUP));
    box(pw, 0, 0);
    mvwprintw(pw, 1, 2, "Enter password (input is visible here for demo):");
    mvwprintw(pw, 3, 2, "Password: ");
    wrefresh(pw);

    echo();
    curs_set(1);
    int rc = mvwgetnstr(pw, 3, 12, out, (int)(maxlen - 1));
    noecho();
    curs_set(0);
    delwin(pw);
    if (rc == ERR) return -1;
    return 0;
}

/* simple variadic message box */
void gui_message(const char *fmt, ...) {
    char buff[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buff, sizeof(buff), fmt, ap);
    va_end(ap);

    int w = (int)strlen(buff) + 8;
    if (w < 36) w = 36;
    int h = 5;
    int y = (LINES - h) / 2, x = (COLS - w) / 2;
    WINDOW *m = newwin(h, w, y, x);
    wbkgd(m, COLOR_PAIR(P_POPUP));
    box(m, 0, 0);
    mvwprintw(m, 2, 4, "%s", buff);
    wrefresh(m);
    wgetch(m);
    delwin(m);
}
