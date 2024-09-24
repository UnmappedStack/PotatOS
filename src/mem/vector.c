#include "include/vector.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../utils/include/printf.h"
#include "include/kheap.h"

Vector* new_vector(uint64_t element_size) {
    Vector *vec = (Vector*) malloc(sizeof(Vector));
    *vec = (Vector) {
        .data_size  = element_size,
        .length     = 0,
        .first_node = 0
    };
    return vec;
}

void vector_push(Vector *vec, uintptr_t val_addr) {
    if (vec->first_node == NULL) {
        vec->first_node = (VectorNode*) malloc(sizeof(VectorNode));
        *vec->first_node = (VectorNode) {
            .index     = 0,
            .next_node = 0,
            .data_addr = val_addr
        };
        vec->length++;
        return;
    }

    VectorNode *this_node = (VectorNode*) vec->first_node;
    while (true)
        if (this_node->next_node != 0)
            this_node = (VectorNode*) this_node->next_node;
        else
            break;
    this_node->next_node = (uintptr_t) malloc(sizeof(VectorNode));
    *((VectorNode*) this_node->next_node) = (VectorNode) {
        .index     = this_node->index,
        .next_node = 0,
        .data_addr = val_addr
    };
    vec->length++;
}

void* vector_at(Vector *vec, size_t idx) {
    if (vec->length <= idx) return NULL;
    VectorNode *this_node   = vec->first_node;
    size_t      current_idx = 0;
    for (; current_idx < idx; current_idx++) this_node = (VectorNode*) this_node->next_node;
    return (void*) this_node->data_addr;
}

void vector_pop(Vector *vec, size_t idx) {
    if (vec->length == 0) {
        kfailf("Cannot pop element %i of vector of size 0.\n", idx);
        return;
    }
    VectorNode *this_node = (VectorNode*) vec->first_node;
    if (idx == 0) {
        vec->first_node = (VectorNode*) this_node->next_node;
        vec->length--;
        return;
    }
    size_t current_idx = 0;
    for (; current_idx < idx; current_idx++) this_node = (VectorNode*) this_node->next_node;
    VectorNode *next_node = (VectorNode*) ((VectorNode*) this_node->next_node)->next_node;
    this_node->next_node = (uintptr_t) next_node;
    vec->length--;
}