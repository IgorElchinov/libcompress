#ifndef VECTOR_UINT8_H
#define VECTOR_UINT8_H

#include <stddef.h>
#include <stdint.h>

typedef struct Vector_uint8 {
    uint8_t *data;
    size_t size;
    size_t capacity;
} Vector_uint8;

extern int vu8_get(Vector_uint8 vector, size_t pos);
extern void vu8_set(Vector_uint8 *vector, int elem, size_t pos);
extern void vu8_push_back(Vector_uint8 *vector, int elem);
extern void vu8_insert(Vector_uint8 *vector, int elem, size_t pos);
extern void vu8_erase(Vector_uint8 *vector, size_t pos);
extern void vu8_print(Vector_uint8 vector);
extern Vector_uint8 vu8_init(size_t size);

#endif
