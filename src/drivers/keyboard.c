#include "include/keyboard.h"
#include "../kernel/kernel.h"
#include "../utils/include/printf.h"
#include "../utils/include/io.h"
#include "include/irq.h"
#include "../cpu/include/idt.h"
#include <stddef.h>
#include "../fs/include/devices.h"
#include "../fs/include/tempfs.h"
#include "../fs/include/vfs.h"
#include "include/pit.h"
#include "../mem/include/kheap.h"

#define PS2_DATA_REGISTER    0x60
#define PS2_STATUS_REGISTER  0x64
#define PS2_COMMAND_REGISTER 0x64

char character_table[] = {
    0,    0,    '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',
    '-',  '=',  0,    0x09, 'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
    'o',  'p',  '[',  ']',  0,    0,    'a',  's',  'd',  'f',  'g',  'h',
    'j',  'k',  'l',  ';',  '\'', '`',  0,    '\\', 'z',  'x',  'c',  'v',
    'b',  'n',  'm',  ',',  '.',  '/',  0,    '*',  0,    ' ',  0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0x1B, 0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0x0E, 0x1C, 0,    0,    0,
    0,    0,    0,    0,    0,    '/',  0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0,
    0,    0,    0,    0,    0,    0,    0,    0x2C
};

char shifted_character_table[] = {
    0,    0,    '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',
    '_',  '+',  0,    0x09, 'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
    'O',  'P',  '{',  '}',  0,    0,    'A',  'S',  'D',  'F',  'G',  'H',
    'J',  'K',  'L',  ':',  '"',  '~',  0,    '|',  'Z',  'X',  'C',  'V',
    'B',  'N',  'M',  '<',  '>',  '?',  0,    '*',  0,    ' ',  0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0x1B, 0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0x0E, 0x1C, 0,    0,    0,
    0,    0,    0,    0,    0,    '?',  0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0,
    0,    0,    0,    0,    0,    0,    0,    0x2C,
};

typedef struct {
    uint64_t input_len;
    bool     currently_reading;
    bool     shifted;
    bool     caps;
    char     *current_buffer;
    uint64_t buffer_size;
} KeyboardData;

KeyboardData *current_input_data;

__attribute__((interrupt))
void keyboard_isr(void*) {
    if (!current_input_data->currently_reading) return;
    if (!(inb(PS2_STATUS_REGISTER) & 0x01)) return;
    uint8_t scancode = inb(PS2_DATA_REGISTER);
    // special cases
    if (scancode & 0x80) { // it's a release, not a press
        if (scancode == 0xAA || scancode == 0xB6) // shift key is released
            current_input_data->shifted = false;
        end_of_interrupt();
        return;
    }
    if (scancode == 0x0E && current_input_data->input_len > 0) { // backspace
        kernel.ch_X -= 8;
        printf(" ");
        kernel.ch_X -= 8;
        current_input_data->input_len--;
        current_input_data->current_buffer[current_input_data->input_len] = 0;
        end_of_interrupt();
        return;
    }
    if (scancode == 0x3A) { // capslock key
        current_input_data->caps = !current_input_data->caps;
        end_of_interrupt();
        return;
    }
    if (scancode == 0x2A || scancode == 0x36) { // one of the shift keys
        current_input_data->shifted = true;
        end_of_interrupt();
        return;
    }
    if (scancode == 0x1C || current_input_data->buffer_size == current_input_data->input_len) { // enter
        current_input_data->current_buffer[current_input_data->input_len] = 0;
        current_input_data->currently_reading = false;
        end_of_interrupt();
        return;
    }
    char ch;
    if (current_input_data->shifted && !current_input_data->caps) {
        ch = shifted_character_table[scancode];
    } else if (current_input_data->shifted && current_input_data->caps) {
        if (character_table[scancode] >= 'a' && character_table[scancode] <= 'z')
            ch = character_table[scancode];
        else
            ch = shifted_character_table[scancode];
    } else if (!current_input_data->shifted && current_input_data->caps) {
        if (character_table[scancode] >= 'a' && character_table[scancode] <= 'z')
            ch = shifted_character_table[scancode];
        else
            ch = character_table[scancode];
    } else {
        ch = character_table[scancode];
    }
    printf("%c", ch);
    current_input_data->current_buffer[current_input_data->input_len] = ch;
    current_input_data->input_len++;
    end_of_interrupt();
    return;
}

int read_ps2_kb(void *filev, char *buffer, size_t max_len) {
    lock_pit();
    Inode *file = (Inode*) filev;
    KeyboardData *kb_data = (KeyboardData*) file->private;
    kb_data->currently_reading = true;
    kb_data->current_buffer    = buffer;
    kb_data->buffer_size       = max_len - 1;
    current_input_data = kb_data;
    while (kb_data->currently_reading) outb(0x80, 0);
    printf("\n");
    kb_data->current_buffer = 0;
    kb_data->input_len      = 0;
    unlock_pit();
    return 0;
}

void open_ps2_kb(void *filev, uint8_t mode) {
    if (mode != MODE_READONLY) {
        kfailf("Cannot open keyboard devices (such as D:/stdin) with write modes. Must be MODE_READONLY.\n");
        return; // yeah nah
    }
    Inode *file = (Inode*) filev;
    KeyboardData *kb_data = (KeyboardData*) malloc(sizeof(KeyboardData));
    file->private = (void*) kb_data;
    *kb_data = (KeyboardData) {0};
    unmask_irq(1);
    asm("sti");
}

void close_ps2_kb(void *filev) {
    Inode *file = (Inode*) filev;
    free(file->private);
}

void init_ps2_keyboard() {
    kstatusf("Initiating PS/2 keyboard...\n");
    DeviceOps kb_dev = (DeviceOps) {
        .read  = &read_ps2_kb,
        .open  = &open_ps2_kb,
        .close = &close_ps2_kb
    };
    create_device("D:/stdin", kb_dev);
    set_IDT_entry(33, &keyboard_isr, 0x8E, (struct IDTEntry*) kernel.idtr.offset);
    unmask_irq(1);
}











