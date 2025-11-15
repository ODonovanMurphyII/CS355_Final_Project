#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define BUF_SIZE 8192

/* Fill fileInfo from path+filename. Returns 0 on success, -1 on error */
int get_file_information(const char* path, const char* filename, file_info* fileInfo)
{
    if (!path || !filename || !fileInfo) return -1;

    char full_path[MAX_PATH_LEN];
    struct stat st;

    if (snprintf(full_path, sizeof(full_path), "%s/%s", path, filename) >= (int)sizeof(full_path)) {
        return -1;
    }

    if (stat(full_path, &st) != 0) {
        return -1;
    }

    strncpy(fileInfo->filename, filename, MAX_FILENAME_LEN-1);
    fileInfo->filename[MAX_FILENAME_LEN-1] = '\0';
    strncpy(fileInfo->fileLocation, path, MAX_PATH_LEN-1);
    fileInfo->fileLocation[MAX_PATH_LEN-1] = '\0';
    fileInfo->mode = st.st_mode;
    fileInfo->userID = st.st_uid;
    fileInfo->groupID = st.st_gid;
    fileInfo->number = -1;
    fileInfo->next = NULL;
    /* filetype left empty for now */
    return 0;
}

/* Returns head of a linked list of file_info for the directory.
 * Caller must free list (see free in example usage). Returns NULL on error.
 */
file_info* get_directory_information(const char* path)
{
    if (!path) return NULL;

    DIR* dir = opendir(path);
    if (!dir) return NULL;

    struct dirent* ent;
    file_info* head = NULL;
    file_info* tail = NULL;
    int idx = 0;

    while ((ent = readdir(dir)) != NULL) {
        /* skip "." and ".." */
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;

        file_info* node = malloc(sizeof(file_info));
        if (!node) {
            // free already allocated nodes
            file_info* cur = head;
            while (cur) { file_info* tmp = cur->next; free(cur); cur = tmp; }
            closedir(dir);
            return NULL;
        }
        memset(node, 0, sizeof(file_info));
        strncpy(node->filename, ent->d_name, MAX_FILENAME_LEN-1);
        node->filename[MAX_FILENAME_LEN-1] = '\0';
        strncpy(node->fileLocation, path, MAX_PATH_LEN-1);
        node->fileLocation[MAX_PATH_LEN-1] = '\0';
        node->number = idx++;
        node->next = NULL;

        /* try to fill stat info */
        char fullpath[MAX_PATH_LEN + MAX_FILENAME_LEN + 2];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, ent->d_name);
        struct stat st;
        if (stat(fullpath, &st) == 0) {
            node->mode = st.st_mode;
            node->userID = st.st_uid;
            node->groupID = st.st_gid;
        } else {
            node->mode = 0;
            node->userID = -1;
            node->groupID = -1;
        }

        if (!head) head = tail = node;
        else { tail->next = node; tail = node; }
    }

    closedir(dir);
    return head;
}

/* Simple XOR encrypt/decrypt: same function does both.
 * Returns 0 on success, -1 on failure.
 */
static int xor_file_with_password(const char* input_path, const char* output_path, const char* password)
{
    if (!input_path || !output_path || !password) return -1;

    FILE* fin = fopen(input_path, "rb");
    if (!fin) return -1;
    FILE* fout = fopen(output_path, "wb");
    if (!fout) { fclose(fin); return -1; }

    size_t passlen = strlen(password);
    if (passlen == 0) { fclose(fin); fclose(fout); return -1; }

    unsigned char buf[BUF_SIZE];
    size_t n;
    size_t offset = 0;
    while ((n = fread(buf, 1, sizeof(buf), fin)) > 0) {
        for (size_t i = 0; i < n; ++i) {
            buf[i] ^= (unsigned char)password[(offset + i) % passlen];
        }
        offset += n;
        if (fwrite(buf, 1, n, fout) != n) {
            fclose(fin); fclose(fout); return -1;
        }
    }

    fclose(fin);
    fclose(fout);
    return 0;
}

int xor_encrypt_file(const char* input_path, const char* output_path, const char* password)
{
    return xor_file_with_password(input_path, output_path, password);
}
int xor_decrypt_file(const char* input_path, const char* output_path, const char* password)
{
    return xor_file_with_password(input_path, output_path, password);
}


