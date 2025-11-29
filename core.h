#ifndef CORE_H
#define CORE_H

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_FILENAME_LEN 255
#define MAX_PATH_LEN 1024
#define FILETYPE_LEN 35
#define MAX_ENTRY_PER_PAGE 10 //previously 2

//File Info Struct
/* forward-declare struct so typedef works */
typedef struct file_info {
    char filename[MAX_FILENAME_LEN];
    char fileLocation[MAX_PATH_LEN];
    char filetype[FILETYPE_LEN];
    int userID, groupID, mode, number, menuNumber, number;
    char action;		
    struct file_info* next;
    struct file_info* prev;
} file_info;

typedef struct page
{ 
file_info* headFileNode;
unsigned int pageNumber;
struct page* next;
struct page* prev;
} page;

/* Function Prototypes */
file_info* get_directory_information(const char* path);

#endif /* CORE_H */
