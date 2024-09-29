#pragma once

#include "vfs.h"
#include "devices.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define FILENODE_DATA_SIZE (4096 - sizeof(uintptr_t))
#define FS_TEMPFS 0

#define FILETYPE_DEVICE 1
#define FILETYPE_REGULAR 2

typedef struct {
    char data[FILENODE_DATA_SIZE];
    uintptr_t next_node;
} FileNode;

typedef struct {
    bool is_dir;
    uintptr_t inode;
    uintptr_t next_node;
} DirEntryNode;

typedef struct {
    char      filename[20];
    uint64_t  size_pages;
    size_t    length;
    bool      is_dir;
    uint8_t   type;
    DeviceOps operations;
    void      *private; // for if it's a device
    union {
        FileNode *file_first_node;
        struct {
            DirEntryNode  *dir_first_node;
            uint64_t      num_entries;
        };
    };
} Inode;

void tempfs_debug(Inode *fs, uint64_t level);
Inode* tempfs_new();
void* tempfs_find_root(void *fs);
void* tempfs_find(void *current_dir, const char *dirname);
void* tempfs_open_dir(void *dir);
void* tempfs_open_file(void *file, uint8_t mode);
void  tempfs_close(void *file);
void tempfs_mkdir(void *current_dir, const char *dirname);
void tempfs_mkfile(void *current_dir, const char *filename);
int tempfs_write(void *filev, char *data, size_t len);
int tempfs_read(void *filev, char *buffer, size_t max_len);
size_t tempfs_length(void *filev);
void tempfs_create_device(void *current_dir, const char *devname, DeviceOps operations);
extern FileSystem TempFS;