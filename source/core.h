#ifndef CORE_H
#define CORE_H

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_FILENAME_LEN 255
#define MAX_PATH_LEN 1024
#define FILETYPE_LEN 35
#define MAX_PASSWORD_LEN 128
#define MAX_ENTRY_PER_PAGE 10

/* forward-declare struct so typedef works */
typedef struct file_info 
{
    char filename[MAX_FILENAME_LEN];
    char fileLocation[MAX_PATH_LEN];
    char filetype[FILETYPE_LEN];
    int userID;
    int groupID;
    int mode;
    int number;
	int menuNumber;
	char action;
    struct file_info* next;
    struct file_info* prev;
}file_info;

typedef struct page 
{
	file_info* headFileNode;
	unsigned int pageNumber;
	unsigned int entryCount;
	struct page* next;
	struct page* prev;
}page;

/* Function Prototypes */
int get_file_information(const char* path, const char* filename, file_info* fileInfo);
/* returns head of list (caller frees) or NULL on error */
file_info* get_directory_information(const char* path);



/* Encryption helpers (simple XOR - reversible) */
int xor_encrypt_file(const char* input_path, const char* output_path, const char* password);
int xor_decrypt_file(const char* input_path, const char* output_path, const char* password);

#endif /* CORE_H */
