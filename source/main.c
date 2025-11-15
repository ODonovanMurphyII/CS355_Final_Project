/* Ncurses File Explorer + XOR encryption demo */
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "core.h"

/* compile: gcc core.c main.c -lncurses -o fileexplorer */

static void free_file_list(file_info* head) {
    while (head) {
        file_info* tmp = head->next;
        free(head);
        head = tmp;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    const char* dirpath = argv[1];
    file_info* list = get_directory_information(dirpath);
    if (!list) {
        fprintf(stderr, "Failed to open directory: %s\n", dirpath);
        return 1;
    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int highlight = 0;
    int ch;
    int count = 0;
    for (file_info* it = list; it; it = it->next) count++;

    while (1) {
        clear();
        mvprintw(0, 0, "Simple File Explorer - %s (q to quit)", dirpath);
        mvprintw(1, 0, "Use arrow keys to navigate. Press Enter to encrypt/decrypt selected file.");
        int row = 3;
        int idx = 0;
        for (file_info* it = list; it; it = it->next, ++idx) {
            if (idx == highlight) {
                attron(A_REVERSE);
                mvprintw(row, 2, "%d: %s", it->number, it->filename);
                attroff(A_REVERSE);
            } else {
                mvprintw(row, 2, "%d: %s", it->number, it->filename);
            }
            row++;
        }
        refresh();

        ch = getch();
        if (ch == 'q') break;
        else if (ch == KEY_UP) {
            if (highlight > 0) highlight--;
        } else if (ch == KEY_DOWN) {
            if (highlight < count - 1) highlight++;
        } else if (ch == '\n' || ch == KEY_ENTER) {
            /* find selected node */
            file_info* sel = list;
            for (int i = 0; i < highlight && sel; ++i) sel = sel->next;
            if (!sel) continue;

            /* show action menu */
            WINDOW* menu = newwin(7, 60, 4, 10);
            box(menu, 0, 0);
            mvwprintw(menu, 1, 2, "Selected: %s", sel->filename);
            mvwprintw(menu, 2, 2, "e - Encrypt    d - Decrypt    c - Cancel");
            mvwprintw(menu, 4, 2, "Enter choice: ");
            wrefresh(menu);
            int choice = wgetch(menu);
            if (choice == 'e' || choice == 'd') {
                /* prompt for password */
                echo();
                curs_set(1);
                char password[128];
                mvwprintw(menu, 4, 2, "Password: ");
                wclrtoeol(menu);
                mvwgetnstr(menu, 4, 12, password, sizeof(password)-1);
                noecho();
                curs_set(0);

                /* build paths */
                char input_path[MAX_PATH_LEN + MAX_FILENAME_LEN + 4];
                char output_path[MAX_PATH_LEN + MAX_FILENAME_LEN + 8];
                snprintf(input_path, sizeof(input_path), "%s/%s", dirpath, sel->filename);

                if (choice == 'e') {
                    snprintf(output_path, sizeof(output_path), "%s/%s.enc", dirpath, sel->filename);
                    int rc = xor_encrypt_file(input_path, output_path, password);
                    if (rc == 0) mvwprintw(menu, 5, 2, "Encrypted -> %s", output_path);
                    else mvwprintw(menu, 5, 2, "Encryption FAILED");
                } else {
                    /* if .enc exists, try to decrypt to filename.dec (or strip .enc) */
                    const char* in_name = sel->filename;
                    if (strlen(in_name) > 4 && strcmp(in_name + strlen(in_name) - 4, ".enc") == 0) {
                        snprintf(output_path, sizeof(output_path), "%s/%.*s.dec", dirpath, (int)(strlen(in_name)-4), in_name);
                    } else {
                        snprintf(output_path, sizeof(output_path), "%s/%s.dec", dirpath, sel->filename);
                    }
                    int rc = xor_decrypt_file(input_path, output_path, password);
                    if (rc == 0) mvwprintw(menu, 5, 2, "Decrypted -> %s", output_path);
                    else mvwprintw(menu, 5, 2, "Decryption FAILED");
                }
                wrefresh(menu);
                wgetch(menu); /* wait for key */
            }
            delwin(menu);
        }
    }

    endwin();
    free_file_list(list);
    return 0;
}
