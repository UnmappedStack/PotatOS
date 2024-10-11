#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint64_t  index;
    uintptr_t next_node;
    uintptr_t data_addr;
} VectorNode;

typedef struct {
    uint64_t   data_size;
    size_t     length;
    VectorNode *first_node;
} Vector;

Vector* new_vector(uint64_t element_size);
void vector_push(Vector *vec, uintptr_t val_addr);
void* vector_at(Vector *vec, size_t idx);
void vector_pop(Vector *vec, size_t idx);
void vector_set(Vector *vec, size_t idx, uintptr_t val);