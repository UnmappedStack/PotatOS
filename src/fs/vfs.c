#include "include/vfs.h"
#include "include/tempfs.h"
#include "../utils/include/printf.h"
#include "../mem/include/kheap.h"
#include "../utils/include/string.h"
#include "../kernel/kernel.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

FileSystem filesystems[1];

void init_vfs() {
    kstatusf("Initialising VFS...");
    ku_memset((uint8_t*)kernel.drives, 0, sizeof(Drive) * 26);
    filesystems[0] = TempFS;
    printf(BGRN " Ok!\n" WHT);
}

int mount(char drive, uint16_t filesystem, bool memory_based, uintptr_t mem_offset, uint64_t disk, uint64_t partition) {
    if (drive >= 'a' && drive <= 'z') drive -= 32;
    if (drive < 'A' || drive > 'Z') return 1;
    uint8_t drive_num = drive - 'A';
    kernel.drives[drive_num] = (Drive) {
        .present    = true,
        .fs         = filesystems[filesystem],
        .in_memory  = memory_based,
    };
    if (memory_based) {
        kernel.drives[drive_num].mem_offset = mem_offset;
    } else {
        kernel.drives[drive_num].disk      = disk;
        kernel.drives[drive_num].partition = partition;
    }
    return 0;
}

int unmount(char drive) {
    if (drive >= 'a' && drive <= 'z') drive -=32;
    if (drive < 'A' || drive > 'Z') return 1;
    kernel.drives[drive - 'A'] = (Drive){0};
    return 0;
}

void close(File *f) {
    uint8_t drive_num = f->drive_char - 'A';
    (kernel.drives[drive_num].fs.close_function)(f->private);
}

File* open(char *path, int flags) {
    char drive = path[0];
    if (drive >= 'a' && drive <= 'z') drive -= 32;
    if (drive < 'A' || drive > 'Z') return NULL;
    if (path[1] != ':' || path[2] != '/') return NULL;
    char *new_path = (char*) malloc(ku_strlen(path));
    ku_memcpy(new_path, path, ku_strlen(path));
    new_path += 3;
    uint8_t drive_num = drive - 'A';
    if (!kernel.drives[drive_num].present) {
        kfailf("Drive is not present.\n");
        free(new_path - 3);
        return NULL;
    }
    uint64_t i = 3;
    void *root = (kernel.drives[drive_num].fs.find_root_function)((void*)kernel.drives[drive_num].mem_offset);
    void *current_obj = (kernel.drives[drive_num].fs.open_dir_function       )(root);
    while (path[i]) {
        size_t this_len = 0;
        for (; new_path[this_len] != '/' && new_path[this_len] != 0; this_len++);
        bool is_dir = new_path[this_len] == '/';
        new_path[this_len] = 0;
        if (is_dir) {
            void *this_dir = (kernel.drives[drive_num].fs.find_function)(current_obj, new_path);
            if (this_dir == NULL) {
                if (flags & O_CREATALL) {
                    (kernel.drives[drive_num].fs.mkdir_function)(current_obj, new_path);
                    this_dir = (kernel.drives[drive_num].fs.find_function)(current_obj, new_path); // look again now it's created.
                } else {
                    kfailf("[FSERR] Cannot open file: directory \"%s\" does not exist.\n", new_path);
                    return NULL;
                }
            }
            current_obj = (kernel.drives[drive_num].fs.open_dir_function)(this_dir);
            new_path += this_len + 1;
            i += this_len + 1;
        } else {
            void *this_file = (kernel.drives[drive_num].fs.find_function)(current_obj, new_path);
            if (this_file == NULL) {
                if (flags & O_CREAT || flags & O_CREATALL) {
                    (kernel.drives[drive_num].fs.mkfile_function)(current_obj, new_path);
                    this_file = (kernel.drives[drive_num].fs.find_function)(current_obj, new_path);
                } else {
                    kfailf("[FSERR] Cannot open file: \"%s\" does not exist.\n", new_path);
                    return NULL;
                }
            }
            current_obj = (kernel.drives[drive_num].fs.open_file_function)(this_file);
            break;
        }
    }
    free((void*)(((uint64_t)new_path) - i));
    File *new_file = (File*) malloc(sizeof(File));
    new_file->drive_char = drive;
    new_file->private    = current_obj;
    return new_file;
}

int mkdir(char *path) {
    char drive = path[0];
    if (drive >= 'a' && drive <= 'z') drive -= 32;
    if (drive < 'A' || drive > 'Z') return 1;
    if (path[1] != ':' || path[2] != '/') return 2;
    char *new_path = (char*) malloc(ku_strlen(path));
    ku_memcpy(new_path, path, ku_strlen(path));
    new_path += 3;
    uint8_t drive_num = drive - 'A';
    uint64_t i = 3;
    void *root = (kernel.drives[drive_num].fs.find_root_function)((void*)kernel.drives[drive_num].mem_offset);
    void *current_obj = (kernel.drives[drive_num].fs.open_dir_function       )(root);
    while (path[i]) {
        size_t this_len = 0;
        for (; new_path[this_len] != '/' && new_path[this_len] != 0; this_len++);
        new_path[this_len] = 0;
        if (new_path[0] == 0) break;
        void *this_dir = (kernel.drives[drive_num].fs.find_function)(current_obj, new_path);
        if (this_dir == NULL) {
            (kernel.drives[drive_num].fs.mkdir_function)(current_obj, new_path);
            this_dir = (kernel.drives[drive_num].fs.find_function)(current_obj, new_path); // look again now it's created.
        }
        current_obj = (kernel.drives[drive_num].fs.open_dir_function)(this_dir);
        new_path += this_len + 1;
        i += this_len + 1;
   }
    free(new_path - i);
    return 0;
}

int mkfile(char *path) {
    close(open(path, O_CREATALL));
}

int write(File *f, char *buffer, size_t size) {
    uint8_t drive_num = f->drive_char - 'A';
    return (kernel.drives[drive_num].fs.write_function)(f->private, buffer, size);
}

int read(File *f, char *buffer, size_t max_len) {
    uint8_t drive_num = f->drive_char - 'A';
    return (kernel.drives[drive_num].fs.read_function)(f->private, buffer, max_len);
}