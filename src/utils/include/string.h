/* Header for string.c (kernel space-only string utilities)
 * Copyright (C) 2024 Jake Steinburger under the MIT license. See the GitHub repo for more information.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void ku_memcpy(char* dest, const char* from, size_t n);
void ku_uint64_to_binary_string(uint64_t num, char *buf);
size_t ku_strlen(const char* str);
int oct2bin(char *str, int size);
void ku_add_char_to_string(char *str, char c);
void ku_remove_last_char(char *str);
void ku_reverse(char str[], int length);
void ku_uint8_to_str(uint8_t num, char* buffer);
void ku_strcpy(char* dest, const char* src);
bool ku_memcmp(const char* str1, const char* str2, uint64_t size);
char* ku_char_to_str(char character);
void ku_uint16_to_string(uint16_t num, char *str);
void ku_uint32_to_string(uint32_t num, char *str);
void ku_uint64_to_string(uint64_t num, char *str);
void ku_memset(void *array, uint64_t value, size_t size);
void ku_uint32_to_hex_string(uint32_t num, char *str);
void ku_uint64_to_hex_string(uint64_t num, char *str);
bool ku_strcmp(const char* str1, const char* str2); 
