#include <stddef.h>
#include <stdint.h>
#include <stddef.h>
#include "include/events.h"
#include "include/tasklist.h"
#include "../kernel/kernel.h"
#include "../mem/include/pmm.h"
#include "../mem/include/paging.h"
#include "../mem/include/kheap.h"
#include "../utils/include/printf.h"
#include "include/spawn.h"
#include "../fs/include/vfs.h"
#include "../utils/include/string.h"

#define ARGV_DATA_ADDR 0x700000001000

typedef struct {
    char id[16];
    uint16_t type;
    uint16_t machine_type;
    uint32_t version;
    uint64_t entry;
    uint64_t program_header_offset;
    uint64_t section_header_offset;
    uint32_t flags;
    uint16_t header_size;
    uint16_t program_header_entry_size;
    uint16_t program_header_entry_count;
    uint16_t section_header_entry_size;
    uint16_t section_header_entry_count;
    uint16_t section_name_string_table_index;
} __attribute__((packed)) elf_file_header;

typedef struct {
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t virtual_address;
    uint64_t rsvd;
    uint64_t size_in_file;
    uint64_t size_in_memory;
    uint64_t align;
} __attribute__((packed)) elf_program_header;

bool validate_elf(elf_file_header *header_addr) {
    if (!ku_memcmp(header_addr->id, "\x7f" "ELF", 4)) {
        kfailf("File does not have magic ELF code at start of header. Invalid ELF file.\n");
        printf("First char: %x\n", ((char*)header_addr)[0]);
        return false;
    }
    if (header_addr->id[5] != 1) {
        kfailf("File appears to be in big endian format, but it must be little endian. Invalid ELF file. Endianness sign: %i\n", header_addr->id[5]);
        return false;
    }
    if (header_addr->id[4] != 2) {
        kfailf("File appears to be 32 bit, but it must be 64 bit. Invalid ELF file.\n");
        return false;
    }
    if (header_addr->machine_type != 0x3E) {
        kfailf("File appears to be for invalid instruction set, must be for x86_64. Invalid ELF file.\n");
        return false;
    }
    return true;
}

// returns index
size_t first_loadable_segment(elf_program_header *program_header_entries, size_t num_entries) {
    size_t    smallest_index    = 0;
    uintptr_t smallest_addr     = 0;
    bool      is_first_loadable = true;
    for (size_t i = 0; i < num_entries; i++) {
        if (program_header_entries[i].type == 1) { // loadable
            if (program_header_entries[i].virtual_address < smallest_addr || is_first_loadable) {
                smallest_index = i;
                smallest_addr = program_header_entries[i].virtual_address;
                is_first_loadable = false;
            }
        }
    }
    return smallest_index;
}

size_t last_loadable_segment(elf_program_header *program_header_entries, size_t num_entries) {
    size_t    largest_index = 0;
    uintptr_t largest_addr = 0;
    for (size_t i = 0; i < num_entries; i++) {
        if (program_header_entries[i].type == 1) { // loadable
            if (program_header_entries[i].virtual_address > largest_addr) {
                largest_index = i;
                largest_addr = program_header_entries[i].virtual_address;
            }
        }
    }
    return largest_index;
}

int spawn(char *path, const char *argv[], size_t argc) {
    File *f = open(path, 0, MODE_READONLY);
    if (f == NULL) return 2;
    size_t flength = file_length(f); 
    char *buffer = (char*) malloc(flength);
    int status = read(f, buffer, flength);
    if (status != 0) {
        close(f);
        return 1;
    }
    close(f);
    elf_file_header    *file_header     = (elf_file_header*) buffer;
    elf_program_header *program_headers = (elf_program_header*)(buffer + file_header->program_header_offset);
    size_t num_program_header_entries   = file_header->program_header_entry_count;
    if (!validate_elf(file_header)) return 1;

    elf_program_header first_segment = program_headers[first_loadable_segment(program_headers, num_program_header_entries)];
    elf_program_header last_segment  = program_headers[last_loadable_segment(program_headers, num_program_header_entries )];

    size_t pages_to_map  = PAGE_ALIGN_UP((last_segment.virtual_address + last_segment.size_in_memory) - first_segment.virtual_address) / 4096;
    char   *copyto_pages = kmalloc(pages_to_map);
    
    for (size_t s = 0; s < num_program_header_entries; s++) {
        if (program_headers[s].type == 1) { // loadable?
            char *copy_from = (char*)((program_headers[s].offset) + (uint64_t) buffer);
            char *copy_to   = (char*) ((copyto_pages + kernel.hhdm) + (program_headers[s].virtual_address - first_segment.virtual_address));
            ku_memcpy(copy_to, copy_from, program_headers[s].size_in_file);
        }
    }
    uint64_t *new_pml4 = init_paging_task();

    map_pages(new_pml4, first_segment.virtual_address, (uint64_t) copyto_pages, pages_to_map, KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE | KERNEL_PFLAG_USER);
    alloc_pages(new_pml4, USER_STACK_ADDR, KERNEL_STACK_PAGES, KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_USER | KERNEL_PFLAG_WRITE); // alloc the user stack
    uint64_t new_pml4_phys = (uint64_t)new_pml4 - kernel.hhdm;

    // give it the arguments
    uint64_t *arg_page = (uint64_t*) ((uint64_t)kmalloc(1) + kernel.hhdm);
    map_pages(new_pml4, ARGV_DATA_ADDR, (uint64_t) arg_page - kernel.hhdm, 2, KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_USER | KERNEL_PFLAG_WRITE);
    
    size_t arg_size = 0;
    for (uint64_t i = 0; i < argc; i++) {
        uint64_t this_arg_len = ku_strlen(argv[i]) + 1;
        ku_memcpy((char*)(((uint64_t) arg_page) + arg_size), argv[i], this_arg_len);
        arg_size += this_arg_len;
    }

    size_t arg_size_hitherto = 0; // fancy words lmao
    for (uint64_t i = 0; i < argc; i++) {
        *((char**)(((uint64_t)arg_page) + arg_size + (i * sizeof(uint64_t)))) = (char*) (((uint64_t)arg_page) + arg_size_hitherto);
        arg_size_hitherto += ku_strlen(argv[i]) + 1;
    }
    Task *new_task = create_task(new_pml4_phys, (uintptr_t) file_header->entry, USER_STACK_PTR, TASK_PRESENT | TASK_FIRST_EXEC);
    new_task->argc = argc;
    new_task->argv = ARGV_DATA_ADDR + arg_size;
    new_task->event_queue = new_event_queue();
    new_task->parent = (uintptr_t) get_task(kernel.tasklist.current_task);
    free(buffer);
    // open standard resource streams
    new_task->resources[0] = open("D:/stdin",  0, MODE_READONLY);
    new_task->resources[1] = open("D:/stdout", O_CREATALL, MODE_READWRITE);
    new_task->resources[2] = open("D:/stderr", O_CREATALL, MODE_READWRITE);
    return 0;
}