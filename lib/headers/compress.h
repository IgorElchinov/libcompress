#ifndef COMPRESS_H
#define COMPRESS_H

#include <stdint.h>
#include "vector_uint8.h"

typedef enum CompressType {
    MOCK_COMPRESS,
    RLE,
    SIMPLE7,
    SIMPLE9,
    FIBONACCI
} CompressType;

enum {
    MAX_CHAR = 255,
    CNT_FIB = 13,
    BITS_IN_UINT8_T = 8,
    SIMPLE9_BLOCK_SIZE = 4,
    SIMPLE9_SERVICE_BITS = 4,
    MAX_MOD = 8
};

extern Vector_uint8 mock_compress(Vector_uint8 data);
extern Vector_uint8 rle_compress(Vector_uint8 data);
extern Vector_uint8 rle_decompress(Vector_uint8 data);
extern Vector_uint8 simple9_compress(Vector_uint8 data);
extern Vector_uint8 simple9_decompress(Vector_uint8 data);
extern Vector_uint8 fibonacci_compress(Vector_uint8 data);
extern Vector_uint8 fibonacci_decompress(Vector_uint8 data);
extern Vector_uint8 compress(void *raw_data, size_t data_len, CompressType mod);
extern Vector_uint8 decompress(void *raw_data, size_t data_len, CompressType mod);

#endif
