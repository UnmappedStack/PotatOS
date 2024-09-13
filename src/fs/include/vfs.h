#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define O_CREAT    0b001
#define O_CREATALL 0b010

typedef struct {
    uint8_t fs_id;
    void* (*find_function     )(void *current_dir, char *dirname);
    void* (*find_root_function)(void *fs  );
    void* (*open_dir_function )(void *dir );
    void* (*open_file_function)(void *file);
    void  (*mkdir_function    )(void *current_dir, char *dirname);
    void  (*mkfile_function   )(void *current_dir, char *filename);
    void  (*close_function    )(void *file);
    int   (*write_function    )(void *file, char *buffer, size_t len);
    int   (*read_function     )(void *file, char *buffer, size_t max_len);
} FileSystem;

typedef struct {
    bool present;
    FileSystem fs;
    bool in_memory;
    union {
        uintptr_t mem_offset;
        struct {
            uint64_t disk;
            uint64_t partition;
        };
    };
} Drive;

typedef struct {
    char drive_char;
    void *private;
} File;

extern FileSystem filesystems[];

void  init_vfs();
int  mount(char drive, uint16_t filesystem, bool memory_based, uintptr_t mem_offset, uint64_t disk, uint64_t partition);
int unmount(char drive);
File* open(char *path, int flags);
void  close(File *f);
int   mkdir(char *path);
int   write(File *f, char *buffer, size_t size);
int   read(File *f, char *buffer, size_t max_len);