/* Ncurses File Explorer + XOR encryption demo */
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "core.h"


/* compile: gcc core.c main.c -lncurses -o fileexplorer */

// Prototypes
static void free_file_list(file_info* head);
file_info* menu_navigation(file_info* head, unsigned int fileCount);

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    const char* dirpath = argv[1];
    int highlight = 0;
    int count = 0;
    int row = 3;
    int idx = 0;
    file_info* headNode = get_directory_information(dirpath);
	file_info* currentFile = headNode;	

    // Invalid Directory Test
    if (!headNode)
    {
        fprintf(stderr, "Failed to open directory: %s\n", dirpath);
        return 1;
    }

    // Getting the file count
    while(currentFile->number >= 0)
    {
		printf("%s\n", currentFile->filename);
		++count;
		currentFile = currentFile->next;
    }
    currentFile = headNode;
	

    // Starting the GUI Up...
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    while (1)
    {
        clear();
        mvprintw(0, 0, "Simple File Explorer | Active Directory:%s (q to quit)", dirpath);
        mvprintw(1, 0, "Use up and down arrow keys to navigate. Press Enter to encrypt/decrypt selected file.\n");
        menu_navigation(headNode, count);

	/*for (currentFile; currentFile->number > 0; currentFile = currentFile->next, ++idx) 
	{
            if (idx == highlight) 
	    {
                attron(A_REVERSE);
                mvprintw(row, 2, "%d: %s", currentFile->number, currentFile->filename);
                attroff(A_REVERSE);
            } else {
                mvprintw(row, 2, "%d: %s", currentFile->number, currentFile->filename);
            }
            row++;
        }
        refresh();
        ch = getch();
        if (ch == 'q') 
	    break;
	else if (ch == KEY_UP)
            if (highlight > 0) 
		highlight--;
        else if (ch == KEY_DOWN)
            if (highlight < count - 1) 
		highlight++;
        else if (ch == '\n' || ch == KEY_ENTER)
	{
            // find selected node 
            file_info* sel = list;
            for (int i = 0; i < highlight && sel; ++i) 
				sel = sel->next;
            if (sel)
	{
				// show action menu 
				WINDOW* menu = newwin(7, 60, 4, 10);
				box(menu, 0, 0);
				mvwprintw(menu, 1, 2, "Selected: %s", sel->filename);
				mvwprintw(menu, 2, 2, "e - Encrypt    d - Decrypt    c - Cancel");
				mvwprintw(menu, 4, 2, "Enter choice: ");
				wrefresh(menu);
				int choice = wgetch(menu);
				if (choice == 'e' || choice == 'd') {
					/* prompt for password 
					echo();
					curs_set(1);
					char password[128];
					mvwprintw(menu, 4, 2, "Password: ");
					wclrtoeol(menu);
					mvwgetnstr(menu, 4, 12, password, sizeof(password)-1);
					noecho();
					curs_set(0);

					/* build paths 
					char input_path[MAX_PATH_LEN + MAX_FILENAME_LEN + 4];
					char output_path[MAX_PATH_LEN + MAX_FILENAME_LEN + 8];
					snprintf(input_path, sizeof(input_path), "%s/%s", dirpath, sel->filename);

					if (choice == 'e') {
						snprintf(output_path, sizeof(output_path), "%s/%s.enc", dirpath, sel->filename);
						//int rc = xor_encrypt_file(input_path, output_path, password);
						if (rc == 0) mvwprintw(menu, 5, 2, "Encrypted -> %s", output_path);
						else mvwprintw(menu, 5, 2, "Encryption FAILED");
					} else {
						/* if .enc exists, try to decrypt to filename.dec (or strip .enc) 
						const char* in_name = sel->filename;
						if (strlen(in_name) > 4 && strcmp(in_name + strlen(in_name) - 4, ".enc") == 0) 
						{
							snprintf(output_path, sizeof(output_path), "%s/%.*s.dec", dirpath, (int)(strlen(in_name)-4), in_name);
						} else {
							snprintf(output_path, sizeof(output_path), "%s/%s.dec", dirpath, sel->filename);
						}
						//int rc = xor_decrypt_file(input_path, output_path, password);
						if (rc == 0) mvwprintw(menu, 5, 2, "Decrypted -> %s", output_path);
						else mvwprintw(menu, 5, 2, "Decryption FAILED");
					}
					wrefresh(menu);
					wgetch(menu); /* wait for key 
				}
			}
            delwin(menu);
        }
    }
    endwin();
    free_file_list(headNode);
    return 0; */
	}
}

file_info* menu_navigation(file_info* head, unsigned int fileCount)
{
	int keypress;
	unsigned int pos = 0;
	file_info* currentFile = head;
	mvprintw(2, 0, "%d/%d: %s", currentFile->number, fileCount, currentFile->filename);
	while(1)
	{
		keypress = getch();
		clear();
		switch(keypress)
		{
			case KEY_DOWN:
				if(pos == 0)
					pos = 0;
				else
					--pos;
				mvprintw(3,0,"Down Key | Position: %d", pos);
				break;
			case KEY_UP:
				if(pos == fileCount-1)
					pos = fileCount-1;
				else
					++pos;
				mvprintw(3,0,"Up Key | Position: %d", pos);
				break;
			default:
				// Do nothing
		}
		currentFile = head;
		while(currentFile->number != pos)									// TODO def needs error handling
			currentFile = currentFile->next;
		mvprintw(2, 0, "Selected File: %d: %s", currentFile->number, currentFile->filename);
		refresh();
	}
	return NULL; // For now
}

static void free_file_list(file_info* head)
{
    file_info* currentFile = head;
	file_info* tmp;
    while(tmp)
    {
        tmp = currentFile->next;
		free(currentFile);
		currentFile = tmp;
    }
}
