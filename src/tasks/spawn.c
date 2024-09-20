#include <stddef.h>
#include <stdint.h>
#include <stddef.h>
#include "../mem/include/kheap.h"
#include "../utils/include/printf.h"
#include "include/spawn.h"
#include "../fs/include/vfs.h"
#include "../utils/include/string.h"

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
    return true;
}

int spawn(char *path) {
    File *f = open(path, 0, MODE_READONLY);
    size_t flength = file_length(f);
    char *buffer = (char*) malloc(flength);
    if (f == NULL) {
        return 2;
    }
    int status = read(f, buffer, flength);
    if (status != 0) {
        close(f);
        return 1;
    }
    close(f);
    printf("File size: %i\n", flength);
    elf_file_header *file_header = (elf_file_header*) buffer;
    printf("File validity: %i\n", validate_elf(file_header));
    free(buffer);
    return 0;
}