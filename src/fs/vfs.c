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
    k_ok();
}

int create_device(char *path, DeviceOps operations) {
    if (path[1] != ':' || path[2] != '/') {
        if (path[0] == '/') {
            char *path_ext = (char*) malloc(ku_strlen(path) + 3);
            ku_memcpy(path_ext + 2, path, ku_strlen(path) + 1);
            path_ext[0] = get_task(kernel.tasklist.current_task)->current_dir[0];
            path_ext[1] = ':';
            path = path_ext;
        } else {
            char *cd = get_task(kernel.tasklist.current_task)->current_dir;
            uint64_t cd_len = ku_strlen(cd);
            uint64_t path_len = ku_strlen(path);
            char *path_ext = (char*) malloc(cd_len + path_len + 1);
            ku_memcpy(path_ext, cd, cd_len);
            ku_memcpy(path_ext + cd_len, path, path_len + 1);
            path = path_ext;
        }
    }
    char drive = path[0];
    if (drive < 'A' || drive > 'Z') return 4;
    char *new_path = (char*) malloc(ku_strlen(path));
    ku_memcpy(new_path, path, ku_strlen(path));
    new_path[ku_strlen(path)] = 0;
    new_path += 3;
    uint8_t drive_num = drive - 'A';
    if (kernel.drives[drive_num].fs.fs_id != FS_TEMPFS) {
        kfailf("[FSERR] Cannot create device on persistant file system, only on TempFS drives. Failed to create new device!\n");
        return 5;
    }
    if (!kernel.drives[drive_num].present) {
        kfailf("Drive is not present.\n");
        free(new_path - 3);
        return 3;
    }
    uint64_t i = 3;
    void *root = (kernel.drives[drive_num].fs.find_root_function      )((void*)kernel.drives[drive_num].mem_offset);
    void *current_obj = (kernel.drives[drive_num].fs.open_dir_function)(root);
    while (path[i]) {
        size_t this_len = 0;
        for (; new_path[this_len] != '/' && new_path[this_len] != 0; this_len++);
        bool is_dir = new_path[this_len] == '/';
        new_path[this_len] = 0;
        if (is_dir) {
            void *this_dir = (kernel.drives[drive_num].fs.find_function)(current_obj, new_path);
            if (this_dir == NULL) {
                kfailf("[FSERR] Cannot open file: directory \"%s\" does not exist.\n", new_path);
                return 2;
            }
            current_obj = (kernel.drives[drive_num].fs.open_dir_function)(this_dir);
            new_path += this_len + 1;
            i += this_len + 1;
        } else {
            void *this_file = (kernel.drives[drive_num].fs.find_function)(current_obj, new_path);
            if (this_file == NULL) {
                (kernel.drives[drive_num].fs.tempfs_create_device)(current_obj, new_path, operations);
                this_file = (kernel.drives[drive_num].fs.find_function)(current_obj, new_path);
            } else {
                kfailf("[FSERR] Cannot create device \"%s\", already exists.\n", new_path);
                return 1;
            }
            break;
        }
    }
    free((void*)(((uint64_t)new_path) - i));
    return 0;
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

uint8_t check_file_type(char *fname) {
    if (ku_strcmp(fname, "stdout"))
        return FTYPE_STDOUT;
    else if (ku_strcmp(fname, "stdin"))
        return FTYPE_STDIN;
    else if (ku_strcmp(fname, "stderr"))
        return FTYPE_STDERR;
    else
        return FTYPE_REGULAR;
}

void change_cd(char *path) {
    Task *current_task = get_task(kernel.tasklist.current_task);
    uint64_t path_len = ku_strlen(path);
    if (path[1] != ':' || path[2] != '/') {
        if (path[0] == '/') {
            char *path_ext = (char*) malloc(path_len + 4);
            ku_memcpy(path_ext + 2, path, path_len + 1);
            path_ext[0] = current_task->current_dir[0];
            path_ext[1] = ':';
            path_ext[path_len + 2] = '/';
            path_ext[path_len + 3] = 0;
            path = path_ext;
        } else {
            char *cd = current_task->current_dir;
            uint64_t cd_len = ku_strlen(cd);
            char *path_ext = (char*) malloc(cd_len + path_len + 1);
            ku_memcpy(path_ext, cd, cd_len);
            ku_memcpy(path_ext + cd_len, path, path_len + 1);
            path_ext[path_len + cd_len] = '/';
            path_ext[path_len + cd_len + 1] = 0;
            path = path_ext;
        }
    }
    free(current_task->current_dir);
    current_task->current_dir = (char*) malloc(ku_strlen(path) + 1);
    ku_memcpy(current_task->current_dir, path, ku_strlen(path) + 1);
}

File* open(char *path, int flags, uint8_t mode) {
    if (path[1] != ':' || path[2] != '/') {
        if (path[0] == '/') {
            char *path_ext = (char*) malloc(ku_strlen(path) + 3);
            ku_memcpy(path_ext + 2, path, ku_strlen(path) + 1);
            path_ext[0] = get_task(kernel.tasklist.current_task)->current_dir[0];
            path_ext[1] = ':';
            path = path_ext;
        } else {
            char *cd = get_task(kernel.tasklist.current_task)->current_dir;
            uint64_t cd_len = ku_strlen(cd);
            uint64_t path_len = ku_strlen(path);
            char *path_ext = (char*) malloc(cd_len + path_len + 1);
            ku_memcpy(path_ext, cd, cd_len);
            ku_memcpy(path_ext + cd_len, path, path_len + 1);
            path = path_ext;
        }
    }
    char drive = path[0];
    if (drive < 'A' || drive > 'Z') return NULL;
    char *new_path = (char*) malloc(ku_strlen(path));
    ku_memcpy(new_path, path, ku_strlen(path));
    new_path[ku_strlen(path)] = 0;
    new_path += 3;
    uint8_t drive_num = drive - 'A';
    if (!kernel.drives[drive_num].present) {
        kfailf("Drive is not present.\n");
        free(new_path - 3);
        return NULL;
    }
    uint64_t i = 3;
    void *root = (kernel.drives[drive_num].fs.find_root_function      )((void*)kernel.drives[drive_num].mem_offset);
    void *current_obj = (kernel.drives[drive_num].fs.open_dir_function)(root);
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
                    return NULL;
                }
            }
            current_obj = (kernel.drives[drive_num].fs.open_file_function)(this_file, mode);
            break;
        }
    }
    free((void*)(((uint64_t)new_path) - i));
    File *new_file = (File*) malloc(sizeof(File));
    new_file->present    = true;
    new_file->mode       = mode;
    new_file->drive_char = drive;
    new_file->private    = current_obj;
    new_file->ftype      = check_file_type(new_path);
    return new_file;
}

int mkdir(char *path) {
    if (path[1] != ':' || path[2] != '/') {
        if (path[0] == '/') {
            char *path_ext = (char*) malloc(ku_strlen(path) + 3);
            ku_memcpy(path_ext + 2, path, ku_strlen(path) + 1);
            path_ext[0] = get_task(kernel.tasklist.current_task)->current_dir[0];
            path_ext[1] = ':';
            path = path_ext;
        } else {
            char *cd = get_task(kernel.tasklist.current_task)->current_dir;
            uint64_t cd_len = ku_strlen(cd);
            uint64_t path_len = ku_strlen(path);
            char *path_ext = (char*) malloc(cd_len + path_len + 1);
            ku_memcpy(path_ext, cd, cd_len);
            ku_memcpy(path_ext + cd_len, path, path_len + 1);
            path = path_ext;
        }
    }
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
    close(open(path, O_CREATALL, MODE_WRITEONLY));
    return 0;
}

size_t file_length(File *f) {
    return (kernel.drives[f->drive_char - 'A'].fs.length_function)(f->private);
}

int write(File *f, char *buffer, size_t size) {
    if (f->mode == MODE_READONLY) {
        kfailf("File opened as read-only, cannot write.\n");
        return 1;
    }
    if (f->ftype == FTYPE_STDOUT)
        printf(buffer);
    uint8_t drive_num = f->drive_char - 'A';
    return (kernel.drives[drive_num].fs.write_function)(f->private, buffer, size);
}

int read(File *f, char *buffer, size_t max_len) {
    if (f->mode == MODE_WRITEONLY) {
        kfailf("File opened as write-only, cannot read.\n");
        return 1;
    }
    uint8_t drive_num = f->drive_char - 'A';
    return (kernel.drives[drive_num].fs.read_function)(f->private, buffer, max_len);
}