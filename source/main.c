/* Ncurses File Explorer + XOR encryption demo */
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "core.h"
#include "GUI.h"


/* compile: gcc core.c main.c -lncurses -o fileexplorer */

// Prototypes
static void free_file_list(file_info* head);
file_info* menu_navigation(page* currentPage, unsigned int pageCount, WINDOW* window);
int action_menu(file_info* targetFile, char* encryptionPassword, WINDOW* subWindow);
int encrypt(file_info* file, char* password);

int main(int argc, char* argv[])
{
    if (argc < 2)
	{
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    const char* dirpath = argv[1];
    int highlight = 0;
    int count = 0;
	unsigned int entryCountForCurPage = 0;
    int row = 3;
    int idx = 0;
	unsigned int pageCount = 1;
	WINDOW* subWindow;
	char encryptionPassword[MAX_PASSWORD_LEN];
	char titleString[512];
	sprintf(titleString,"Simple File Explorer and file encryptor | Active Directory:%s (q to quit)\n",dirpath);
	char subtitleString[] = "Use up and down arrow keys to navigate. Press Enter to encrypt/decrypt selected file.\n";
    file_info* headNode = get_directory_information(dirpath);
	file_info* currentFile = headNode;
	file_info* selectedFile = NULL;
	page firstMenuPage;
	page* currentPage = &firstMenuPage;

	// Invalid Directory Test
    if (!headNode)
    {
        fprintf(stderr, "Failed to open directory: %s\n", dirpath);
        return 1;
    }

    // Getting the file count and setting up pages
	firstMenuPage.headFileNode = currentFile;
	firstMenuPage.prev = NULL;
	firstMenuPage.next = NULL;
	firstMenuPage.pageNumber = 1;
    while(currentFile->number >= 0)
    {
		++count;
		++entryCountForCurPage;
		//printf("%d\n", entryCountForCurPage);
		//getc(stdin);
		currentFile = currentFile->next;
		if(currentFile->number >= 0 && (count % MAX_ENTRY_PER_PAGE) == 0)			// Time to setup a new page
		{
			currentPage->entryCount = entryCountForCurPage;
			entryCountForCurPage = 0;
			currentPage->next = malloc(sizeof(page));
			currentPage->next->prev = currentPage;
			currentPage = currentPage->next;
			currentPage->next = NULL;
			currentPage->headFileNode = currentFile;
			++pageCount;
			currentPage->pageNumber = pageCount;
		}
    }
	currentPage = &firstMenuPage;
	//printf("%d\n", entryCountForCurPage);
	//getc(stdin);

#if 0	// Debug test for page creation
	while(currentPage->next != NULL)
	{
		printf("%s\n",currentPage->headFileNode->filename);
		getc(stdin);
		currentPage = currentPage->next;
	}
	printf("%s\n", currentPage->headFileNode->filename);
	printf("%d\n", pageCount);
	getc(stdin);
#endif

	// Setting up pages
	int i = 0;

    // Starting the GUI Up...
	gui_init();
	subWindow = newwin(SUB_WINDOW_HEIGHT, SUB_WINDOW_WIDTH,
				(LINES-SUB_WINDOW_HEIGHT)/2, (COLS-SUB_WINDOW_WIDTH)/2);
	keypad(subWindow,TRUE);
	wbkgd(subWindow, COLOR_PAIR(CP_SUBWINDOW));
	box(subWindow, 0,0);
	wrefresh(subWindow);

    while (1)
    {
        mvprintw(LINES/8,(COLS - strlen(titleString))/2,titleString);
        mvprintw((LINES/8)+1,(COLS - strlen(subtitleString))/2,subtitleString);
        selectedFile = menu_navigation(currentPage,pageCount,subWindow);			// Returns selected file (node)
		if(selectedFile != NULL)
		{
			if(action_menu(selectedFile, encryptionPassword, subWindow) == 1)
			{
				switch(selectedFile->action)
				{
					case 'e':
						encrypt(selectedFile, encryptionPassword); // Run encryption function on file
					case 'd':
						encrypt(selectedFile, encryptionPassword); // Run decryption function on file
					default:
						; // No action to take place here
				}
				selectedFile->action = 0;								// Reset after doing the work
				encryptionPassword[0] = '\0';							// ...same
			}
		} else {
			mvwprintw(subWindow,getmaxy(subWindow)-10,1,"Memory Error Detected - Returning to Main Menu");
			wrefresh(subWindow);
			napms(1000);
		}
	}
    endwin();
    free_file_list(headNode);
    return 0;
}

// Encryption/Decryption Function - Returns -1 on failure
int encrypt(file_info* file, char* password)
{
	char tempFilePath[MAX_PATH_LEN];
    char buffer;
	FILE* outputFile;
	FILE* inputFile;
	size_t passlen;
	int i = 0;
	int n = 0;

	// Opening input file 
	inputFile = fopen(file->filename, "wb");				// Opening with write access so we can delete later
	if(inputFile == NULL)
	{
		// TODO Print an error message here
		exit(EXIT_FAILURE);
	}

	// Creating temporary file to work with
	snprintf(tempFilePath, sizeof(file->filename), "%s.enc", file->filename); //TODO needs error checking and overflow prevention
	outputFile = fopen(tempFilePath, "wb");
	if(outputFile == NULL)
	{
		// TODO Print an error message here
		exit(EXIT_FAILURE);
	}

	passlen = strlen(password);
	if(passlen == 0)
	{
		fclose(inputFile);
		fclose(outputFile);
		// TODO Print an error message
		return -1;
	}

	// Starting encryption/decryption
	while ((n = fread(&buffer, 1, 1, inputFile)) > 0) 
	{
        buffer ^= (unsigned char)password[(i) % passlen];		// note to Sadie: why use the offset? % passlen is very clever!
        //offset += n;
        fwrite(&buffer, 1, 1, outputFile) != n; 		// TODO need to check for errors
    }

	// Deleting the original file and replacing it with the encrypted version
	remove(file->filename);
	rename(tempFilePath,  file->filename);				// TODO needs error checking

	fclose(inputFile);
	fclose(outputFile);
	return 0;
}

// Blocking function
file_info* menu_navigation(page* currentPage, unsigned int pageCount, WINDOW* window)
{
	int keypress;
	int i = 0;
	int y = 2;
	unsigned int pos = 0;
	unsigned int maxCursorPos = 0;
	int maxY = 0;
	file_info* currentFile = currentPage->headFileNode;
	wclear(window);
	box(window,0,0);
	refresh();
	wrefresh(window);				// Set focus to window before enabling cursor
	while(currentFile->number != -1 && i < MAX_ENTRY_PER_PAGE)			// TODO could probably be a subroutine
	{
		mvwprintw(window,y,2, "%d) %s", i, currentFile->filename);
		currentFile = currentFile->next;
		wrefresh(window);
		++i;
		++y;
		++maxCursorPos;
	}
	--maxCursorPos;
	maxY = getmaxy(window);
	mvwprintw(window,maxY-2,1,"Page:%d/%d| EC: %d | Prev %d Files:<- | Next %d Files:->",currentPage->pageNumber,
	pageCount,maxCursorPos+1, MAX_ENTRY_PER_PAGE, MAX_ENTRY_PER_PAGE);
	y = 2;
	curs_set(2);
	wmove(window,y,2);
	wrefresh(window);
	while(1)
	{
		keypress = wgetch(window);
		switch(keypress)
		{
			case KEY_UP:
				if(pos == 0)
				{
					pos = 0;
				} else {
					--pos;
					--y;
				}
				break;
			case KEY_DOWN:
				if(pos == maxCursorPos)
				{
					pos = maxCursorPos;
				} else {
					++pos;
					++y;
#if 0
				mvwprintw(window,1,5, "Found File %s | Pos: %d | Page Entry Count: %d", currentPage->headFileNode->filename,
								pos, maxCursorPos);
				wrefresh(window);
				wgetch(window);
#endif
				}
				break;
			case KEY_ENTER:
			case '\n':
				currentFile = currentPage->headFileNode;
				// Determing where each file is on this page
				int menuMap = 0;
				do
				{
					currentFile->menuNumber = menuMap;
					currentFile = currentFile->next;
					++menuMap;
				}while(currentFile->number != -1);
#if 0
				mvwprintw(window,1,10, "Filename: %s | Pos: %d | Menu Map: %d", currentFile->filename, pos, menuMap);
				wrefresh(window);
				wgetch(window);
#endif
				currentFile = currentPage->headFileNode;
				while(currentFile->menuNumber != pos)
				{
					currentFile = currentFile->next;
#if 0
					mvwprintw(window,1,5, "Current File %s | Pos: %d | Current menuNumber: %d", currentFile->filename,
								pos, currentFile->menuNumber);
					wrefresh(window);
					wgetch(window);
#endif
				}
#if 0
				mvwprintw(window,1,5, "Found File %s | Pos: %d | Current menuNumber: %d", currentFile->filename,
								pos, currentFile->menuNumber);
				wrefresh(window);
				wgetch(window);
#endif
				return currentFile;
				break;
			case KEY_LEFT:
				if(currentPage->prev != NULL)
				{
					menu_navigation(currentPage->prev,pageCount,window);
					return NULL;
				}
				break;
			case KEY_RIGHT:
				if(currentPage->next != NULL)
				{
					menu_navigation(currentPage->next,pageCount,window);
					return NULL;
				}
				break;
			case 'q':								// Cheap way to exit out for now. Letting the OS do the heavy lifting
				endwin();
				exit(0);
			default:
				// Do nothing
		}
#if 0
		mvwprintw(window,1,10, "Y:%d | POS: %d", y, pos);
#endif
		wmove(window,y,2);
		wrefresh(window);
	}
	return NULL; // if we get here, there is an error
}

// Blocking function
int action_menu(file_info* targetFile, char* encryptionPassword, WINDOW* menuWindow)
{
	int choice;
	char password[MAX_PASSWORD_LEN];
	wclear(menuWindow);
	box(menuWindow, 0,0);
	wrefresh(menuWindow);
	mvwprintw(menuWindow, 1, 2, "Selected: %s", targetFile->filename);
	mvwprintw(menuWindow, 2, 2, "e - Encrypt | d - Decrypt | c - Cancel");
	mvwprintw(menuWindow, 4, 2, "Enter choice:");
	wrefresh(menuWindow);
	while(1)
	{
		choice = wgetch(menuWindow);
		if (choice == 'e' || choice == 'd')
		{
			// prompt for password
			echo();
			curs_set(2);
			mvwprintw(menuWindow, 4, 2, "Password: ");
			wclrtoeol(menuWindow);
			mvwgetnstr(menuWindow, 4, 12, password, sizeof(password)-1);
			noecho();
			wrefresh(menuWindow);
			targetFile->action = choice;		// So we can keep track of what we're doing to the file
			strcpy(encryptionPassword, password);
			return 1;
		} else if (choice == 'c') {
			return 0;
		} else if (choice == 'q') {
			endwin();
			exit(0);
		} else {
			mvwprintw(menuWindow, 4, 2, "Invalid Selection - Returning to the Main Menu");
			wrefresh(menuWindow);
			napms(2000);
			return -1;
		}
	}
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
