#include "../utils/include/printf.h"
#include "include/vfs.h"
#include "include/tempfs.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../utils/include/string.h"
#include "../mem/include/kheap.h"

FileSystem TempFS = {
    .fs_id = FS_TEMPFS,
    .find_function        = &tempfs_find,
    .find_root_function   = &tempfs_find_root,
    .open_dir_function    = &tempfs_open_dir,
    .open_file_function   = &tempfs_open_file,
    .mkdir_function       = &tempfs_mkdir,
    .mkfile_function      = &tempfs_mkfile,
    .close_function       = &tempfs_close,
    .write_function       = &tempfs_write,
    .read_function        = &tempfs_read,
    .length_function      = &tempfs_length,
    .tempfs_create_device = &tempfs_create_device
};

void tempfs_mkdir_nonlinear(void *current_dir, const char *dirname, DirEntryNode *first_dir_entry);

Inode* tempfs_new() {
    Inode *root_inode = (Inode*) malloc(sizeof(Inode));
    // create two entries at the start, `.` and `..`
    // make the `.` entry
    root_inode->dir_first_node = (DirEntryNode*) malloc(sizeof(DirEntryNode));
    ((DirEntryNode*)root_inode->dir_first_node)->inode  = (uintptr_t) malloc(sizeof(Inode));
    ((DirEntryNode*)root_inode->dir_first_node)->is_dir = true;
    Inode *first_inode = (Inode*) ((DirEntryNode*)root_inode->dir_first_node)->inode;
    first_inode->filename[0] = '.';
    first_inode->filename[1] = 0;
    first_inode->is_dir = true;
    first_inode->dir_first_node = root_inode->dir_first_node;
    first_inode->num_entries = 2;
    // make the `..` entry (identical for the root to `.`)
    tempfs_mkdir_nonlinear((void*) root_inode, "..", root_inode->dir_first_node);
    root_inode->is_dir         = true;
    return root_inode;
}

Inode* tempfs_addentry(Inode *current_dir, const char *dirname, bool is_dir) {
    DirEntryNode *this_entry = current_dir->dir_first_node;
    uintptr_t inode_new = (uintptr_t) malloc(sizeof(Inode));
    DirEntryNode *new_entry = (DirEntryNode*) malloc(sizeof(DirEntryNode));
    ku_memset((void*) new_entry, 0, sizeof(DirEntryNode));
    ku_memset((void*) inode_new, 0, sizeof(Inode));
    if (this_entry != NULL) {
        for (size_t e = 0; e < current_dir->num_entries; e++)
            this_entry = (DirEntryNode*) this_entry->next_node;
        this_entry->next_node = (uintptr_t)new_entry;
    } else {
        current_dir->dir_first_node = new_entry;
    }
    ku_memcpy(((Inode*)inode_new)->filename, dirname, ku_strlen(dirname));
    ((Inode*)inode_new)->is_dir = is_dir;
    new_entry->inode = inode_new;
    current_dir->num_entries++;
    return (Inode*) new_entry->inode;
}

// like mkdir, but you can choose the first dir entry that it points to, like an existing one
void tempfs_mkdir_nonlinear(void *current_dir, const char *dirname, DirEntryNode *first_dir_entry) {
    Inode *new_inode = tempfs_addentry((Inode*)current_dir, dirname, true);
    new_inode->dir_first_node = first_dir_entry;
}

void tempfs_mkdir(void *current_dir, const char *dirname) {
    Inode *new_inode = tempfs_addentry((Inode*)current_dir, dirname, true);
    // create two entries at the start, `.` and `..`
    // make the `.` entry
    new_inode->dir_first_node = (DirEntryNode*) malloc(sizeof(DirEntryNode));
    ((DirEntryNode*)new_inode->dir_first_node)->inode  = (uintptr_t) malloc(sizeof(Inode));
    ku_memset((uint8_t*)((DirEntryNode*)new_inode->dir_first_node)->inode, 0, sizeof(Inode));
    ((DirEntryNode*)new_inode->dir_first_node)->is_dir = true;
    Inode *first_inode = (Inode*) ((DirEntryNode*)new_inode->dir_first_node)->inode;
    ku_memset((uint8_t*)((DirEntryNode*)new_inode->dir_first_node)->inode, 0, sizeof(Inode));
    first_inode->filename[0] = '.';
    first_inode->is_dir = true;
    first_inode->dir_first_node = new_inode->dir_first_node;
    first_inode->num_entries = 2;
    // make the `..` entry
    tempfs_mkdir_nonlinear((void*) new_inode, "..", ((Inode*) current_dir)->dir_first_node);
}

void tempfs_mkfile(void *current_dir, const char *filename) {
    tempfs_addentry((Inode*)current_dir, filename, false);
}

void tempfs_create_device(void *current_dir, const char *devname, DeviceOps operations) {
    Inode *new_device      = tempfs_addentry((Inode*) current_dir, devname, false);
    new_device->type       = FILETYPE_DEVICE;
    new_device->operations = operations;
}

void* tempfs_find(void *current_dir, const char *dirname) {
    DirEntryNode *this_entry = ((Inode*)current_dir)->dir_first_node;
    Inode        *this_inode = (Inode*) this_entry->inode;
    while (this_entry != NULL) {
        if (ku_strcmp(this_inode->filename, dirname)) {
            return (Inode*) this_entry->inode;
        }
        this_entry = (DirEntryNode*) this_entry->next_node;
        if (this_entry != NULL) {
            this_inode = (Inode*) this_entry->inode;
        }
    }
    return (void*) this_entry;
}

void* tempfs_find_root(void *fs) {
    return fs;
}

size_t tempfs_length(void *filev) {
    return ((Inode*) filev)->length;
}

int tempfs_write(void *filev, char *data, size_t len) {
    Inode *file = (Inode*) filev;
    if (file->type == FILETYPE_DEVICE) return file->operations.write(file, data, len);
    file->length = len;
    if (file->file_first_node == NULL)
        file->file_first_node = (FileNode*) malloc(sizeof(FileNode));
    FileNode *this_fnode = file->file_first_node;
    uint64_t len_left = len;
    for (size_t i = 0; i < len; i += FILENODE_DATA_SIZE) { 
        if (this_fnode->next_node == 0) {
            this_fnode->next_node = (uintptr_t) malloc(sizeof(FileNode));
            ((FileNode*)this_fnode->next_node)->next_node = 0;
        }
        uint64_t amount_to_copy = (len_left > FILENODE_DATA_SIZE) ? FILENODE_DATA_SIZE : len_left;
        ku_memcpy(this_fnode->data, data + i, amount_to_copy);
        this_fnode = (FileNode*) this_fnode->next_node;
        len_left -= FILENODE_DATA_SIZE;
    }
    return 0;
}

int tempfs_read(void *filev, char *buffer, size_t max_len) {
    Inode *file = (Inode*) filev;
    if (file->type == FILETYPE_DEVICE) return file->operations.read(file, buffer, max_len);
    FileNode *this_fnode = file->file_first_node;
    uint64_t len_left = max_len;
    for (size_t i = 0; i < max_len && this_fnode != NULL; i += FILENODE_DATA_SIZE) {
        uint64_t amount_to_copy = (len_left > FILENODE_DATA_SIZE) ? FILENODE_DATA_SIZE : len_left;
        ku_memcpy(buffer + i, this_fnode->data, amount_to_copy);
        this_fnode = (FileNode*) this_fnode->next_node;
        len_left -= FILENODE_DATA_SIZE;
    }
    return 0; 
}

/*
 * this is seriously the most complication function I have ever seen.
 * even torvalds would be stumped by how the hell tempfs_open works xD
 * the reason that these basically useless functions exist is purely to work with the VFS of this dumb kernel.
 */
void* tempfs_open_dir(void *dir) {
    if (((Inode*)dir)->is_dir)
        return dir;
    else
        return NULL;
}

void* tempfs_open_file(void *file) {
    if (!((Inode*)file)->is_dir)
        return file;
    else
        return NULL;
}

// and something even more high tech...
void tempfs_close(void *inode) {}
