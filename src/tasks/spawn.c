#include <stddef.h>
#include <stdint.h>
#include <stddef.h>
#include "../utils/include/printf.h"
#include "include/spawn.h"
#include "../fs/include/vfs.h"
#include "../utils/include/string.h"

struct elf_file_header {
    char id[16];
    uint16_t type;
    uint16_t machine_type;
    uint32_t version;
    uint64_t entry;
    uint64_t program_header_offset;
    uint64_t section_header_offset;
    uint32_t flags;
    uint16_t header_hize;
    uint16_t program_header_entry_size;
    uint16_t program_header_entry_count;
    uint16_t section_header_entry_size;
    uint16_t section_header_entry_count;
    uint16_t section_name_string_table_index;
} __attribute__ ((packed));

struct elf_program_header{
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t virtual_address;
    uint64_t rsvd;
    uint64_t size_in_file;
    uint64_t size_in_memory;
    uint64_t align;
} __attribute__ ((packed));

bool validate_elf(char *elf_addr) {
    return ku_memcmp(elf_addr, "\x7f" "ELF", 4);
}

int spawn(char *path) {
    char buffer[5];
    File *f = open(path, 0, MODE_READONLY);
    if (f == NULL) {
        return 2;
    }
    int status = read(f, buffer, 4);
    if (status != 0) {
        close(f);
        return 1;
    }
    printf("File size: %i bytes\n", file_length(f));
    close(f);
    printf("ELF validity: %i\n", validate_elf(buffer));
    return 0;
}