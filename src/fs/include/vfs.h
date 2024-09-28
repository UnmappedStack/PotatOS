#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define O_CREAT    0b001
#define O_CREATALL 0b010

#define MODE_READONLY  0b01
#define MODE_WRITEONLY 0b10
#define MODE_READWRITE 0b11

#define FTYPE_REGULAR  0b001
#define FTYPE_STDOUT   0b010
#define FTYPE_STDIN    0b011
#define FTYPE_STDERR   0b100

typedef struct {
    uint8_t fs_id;
    void*  (*find_function     )(void *current_dir, const char *dirname);
    void*  (*find_root_function)(void *fs  );
    void*  (*open_dir_function )(void *dir );
    void*  (*open_file_function)(void *file);
    void   (*mkdir_function    )(void *current_dir, const char *dirname);
    void   (*mkfile_function   )(void *current_dir, const char *filename);
    void   (*close_function    )(void *file);
    int    (*write_function    )(void *file, char *buffer, size_t len);
    int    (*read_function     )(void *file, char *buffer, size_t max_len);
    size_t (*length_function   )(void *file);
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
    bool present;
    uint8_t ftype;
    size_t length; // (bytes)
    char drive_char;
    uint8_t mode;
    void *private;
} File;

extern FileSystem filesystems[];

void  init_vfs();
void change_cd(char *path);
int  mount(char drive, uint16_t filesystem, bool memory_based, uintptr_t mem_offset, uint64_t disk, uint64_t partition);
int unmount(char drive);
File* open(char *path, int flags, uint8_t mode);
void  close(File *f);
int   mkdir(char *path);
int   write(File *f, char *buffer, size_t size);
int   read(File *f, char *buffer, size_t max_len);
size_t file_length(File *f);