#include "vector_uint8.h"
#include <stdlib.h>
#include <stdio.h>

void
vu8_check_pos(Vector_uint8 vector, size_t pos) {
    if (pos < 0 || pos >= vector.size) {
        fprintf(stderr, "List index is out of range\n");
        exit(1);
    }
}

int
vu8_get(Vector_uint8 vector, size_t pos) {
    vu8_check_pos(vector, pos);
    return vector.data[pos];
}

void
vu8_set(Vector_uint8 *vector, int elem, size_t pos) {
    vu8_check_pos(*vector, pos);
    vector->data[pos] = elem;
    return;
}

void
vu8_push_back(Vector_uint8 *vector, int elem) {
    if (vector->capacity == 0) {
        vector->capacity = 1;
        vector->size = 1;
        vector->data = calloc(1, sizeof(*vector->data));
        return;
    }
    if (vector->size >= vector->capacity) {
        vector->data = realloc(vector->data, 2 * vector->capacity * sizeof(*vector->data));
        if (vector->data == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            exit(1);
        }
        vector->capacity *= 2;
    }
    vector->data[vector->size] = elem;
    vector->size++;
    return;
}


void
vu8_insert(Vector_uint8 *vector, int elem, size_t pos) {
    vu8_push_back(vector, elem);
    vu8_check_pos(*vector, pos);
    for (size_t i = vector->size - 1; i > pos; --i) {
        vu8_set(vector, vu8_get(*vector, i - 1), i);
    }
    vu8_set(vector, elem, pos);
    return;
}

void
vu8_erase(Vector_uint8 *vector, size_t pos) {
    vu8_check_pos(*vector, pos);
    for (size_t i = pos; i < vector->size - 1; ++i) {
        vu8_set(vector, vu8_get(*vector, i + 1), i);
    }
    vector->size--;
    return;
}

void
vu8_print(Vector_uint8 vector) {
    for (size_t i = 0; i < vector.size; ++i) {
        printf("%d ", vu8_get(vector, i));
    }
    printf("\n");
    return;
}

Vector_uint8
vu8_init(size_t size) {
    Vector_uint8 vector;
    vector.size = size;
    if (size == 0) {
        size = 1;
    }
    vector.data = calloc(size, sizeof(*vector.data));
    if (vector.data == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    vector.capacity = size;
    return vector;
}
