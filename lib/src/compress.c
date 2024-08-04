#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "compress.h"

Vector_uint8
mock_compress(Vector_uint8 data) {
    Vector_uint8 result = vu8_init(data.size);
    for (size_t i = 0; i < data.size; ++i) {
        vu8_set(&result, vu8_get(data, i), i);
    }
    return result;
}

Vector_uint8
rle_compress(Vector_uint8 data) {
    Vector_uint8 result = {0};
    Vector_uint8 tmp = vu8_init(data.size);
    int tmp_ptr = 0, cnt_common = 1;
    for (size_t i = 0; i < data.size; ++i) {
        if (i == 0) {
            continue;
        }
        if (vu8_get(data, i) == vu8_get(data, i - 1) && cnt_common < MAX_CHAR) {
            cnt_common++;
        } else {
            if (tmp_ptr + 2 >= data.size) {
                fprintf(stderr, "Result ineffective\n");
                free(tmp.data);
                return result;
            }
            vu8_set(&tmp, cnt_common, tmp_ptr++);
            vu8_set(&tmp, vu8_get(data, i - 1), tmp_ptr++);
            cnt_common = 1;
        }
    }
    result = vu8_init(0);
    for (long long i = 0; i < tmp_ptr; ++i) {
        vu8_push_back(&result, vu8_get(tmp, i));
    }
    free(tmp.data);
    return result;
}

Vector_uint8
rle_decompress(Vector_uint8 data) {
    Vector_uint8 result = {0};
    if (data.size % 2 == 1) {
        fprintf(stderr, "Invalid input data\n");
        return result;
    }
    result = vu8_init(0);
    uint8_t cnt = 0, elem;
    for (size_t i = 0; i < data.size; i += 2) {
        cnt = vu8_get(data, i);
        elem = vu8_get(data, i + 1);
        for (size_t j = 0; j < cnt; ++j) {
            vu8_push_back(&result, elem);
        }
    }
    return result;
}

uint8_t
get_bit(uint8_t x, size_t bit) {
     return (x >> bit) & 1;
}

uint8_t
set_bit_c(uint8_t *n, size_t bit, int num) {
    if (num != 0) {
        *n = *n | (1 << bit);
    } else {
        *n = *n & ~(1 << bit);
    }
    return *n;
}

uint32_t
set_bit(uint32_t *n, size_t bit, int num) {
    if (num != 0) {
        *n = *n | (1 << bit);
    } else {
        *n = *n & ~(1 << bit);
    }
    return *n;
}

uint8_t
get_bits(uint32_t *n, size_t bit, size_t len) {
    return (*n >> bit) & ((1 << len) - 1);
}

const size_t num_size[] = {1, 2, 3, 4, 5, 7, 8, 14, 28};
const size_t max_block_len[] = {28, 12, 9, 7, 5, 4, 3, 2, 1};
const uint8_t max_num[] = {1, 3, 7, 15, 31, 127, 255, 255, 255};

void
add_simple9_block(Vector_uint8 *v, int mod, uint8_t *first_num, size_t cnt) {
    size_t bits_in_block = SIMPLE9_BLOCK_SIZE * 8;
    for (size_t i = 0; i < SIMPLE9_BLOCK_SIZE; ++i) {
        vu8_push_back(v, 0);
    }
    uint32_t *block = (uint32_t *)&v->data[v->size - SIMPLE9_BLOCK_SIZE];
    size_t cur_bit;
    if (mod < 0 || mod > MAX_MOD) {
        fprintf(stderr, "Unknown block type\n");
        return;
    }
    while (mod <= MAX_MOD && cnt <= max_block_len[mod]) {
        mod++;
    }
    --mod;
    *block |= mod << (bits_in_block - SIMPLE9_SERVICE_BITS);
    cur_bit = bits_in_block - SIMPLE9_SERVICE_BITS;
    for (size_t i = 0; i < cnt; ++i) {
        cur_bit -= num_size[mod];
        for (size_t j = 0; j < num_size[mod]; ++j) {
            set_bit(block, cur_bit + j, get_bit(first_num[i], j));
        }
    }
}

Vector_uint8
simple9_compress(Vector_uint8 data) {
    Vector_uint8 result = vu8_init(0);
    int cur_mod  = 0;
    size_t cur_block_len = 0;
    size_t first_num = 0;
    for (size_t i = 0; i < data.size; ++i) {
        if (cur_block_len >= max_block_len[cur_mod]) {
            add_simple9_block(&result, cur_mod, &data.data[first_num], cur_block_len);
            cur_mod = 0;
            first_num = i;
            cur_block_len = 0;
        }
        if (vu8_get(data, i) <= max_num[cur_mod]) {
            cur_block_len++;
        } else {
            int new_mod = cur_mod;
            for (; new_mod <= MAX_MOD && vu8_get(data, i) > max_num[new_mod]; ++new_mod) { }
            if (vu8_get(data, i) > max_num[new_mod]) {
                free(result.data);
                fprintf(stderr, "Invalid input data\n");
                return result;
            }
            if (cur_block_len + 1 > max_block_len[new_mod]) {
                add_simple9_block(&result, cur_mod, &data.data[first_num], cur_block_len);
                first_num = i;
                cur_block_len = 1;
            } else {
                cur_block_len++;
            }
            cur_mod = new_mod;
        }
    }
    add_simple9_block(&result, cur_mod, &data.data[first_num], cur_block_len);
    return result;
}

Vector_uint8
simple9_decompress(Vector_uint8 data) {
    Vector_uint8 result = {0};
    if (data.size % 4 != 0) {
        fprintf(stderr, "Invalid input data\n");
        return result;
    }
    result = vu8_init(0);
    size_t bits_in_simple7_block = 8 * SIMPLE9_BLOCK_SIZE;
    for (size_t i = 0; i < data.size; i += 4) {
        uint32_t *cur_num = (uint32_t *)&data.data[i];
        int mod = get_bits(cur_num, bits_in_simple7_block - SIMPLE9_SERVICE_BITS, SIMPLE9_SERVICE_BITS);
        if (mod > MAX_MOD) {
            fprintf(stderr, "Unknown block type: %d", mod);
            fflush(stderr);
            free(result.data);
            result.data = NULL;
            return result;
        }
        for (size_t j = 1;  j <= max_block_len[mod]; j++) {
            int cur_val = get_bits(cur_num, bits_in_simple7_block - SIMPLE9_SERVICE_BITS - j * num_size[mod], num_size[mod]);
            vu8_push_back(&result, cur_val);
        }
    }
    return result;
}

const uint8_t fib_nums[] = {0, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233};

void
upd_bit(size_t *cur_bit, Vector_uint8 *vector) {
    if (*cur_bit == 0) {
        vu8_push_back(vector, 0);
        *cur_bit = BITS_IN_UINT8_T - 1;
    } else {
        (*cur_bit)--;
    }
    return;
}

void
add_fibonacci_block(Vector_uint8 *result, Vector_uint8 *ans, size_t *cur_bit) {
    for (size_t i = 0; i < ans->size; ++i) {
        upd_bit(cur_bit, result);
        set_bit_c(&result->data[result->size - 1], *cur_bit, vu8_get(*ans, i));
    }
    while (ans->size > 0) {
        vu8_erase(ans, 0);
    }
}

Vector_uint8
fibonacci_compress(Vector_uint8 data) {
    Vector_uint8 result = {0};
    Vector_uint8 cur_block = vu8_init(0);
    size_t cur_bit = 0;
    for (size_t i = 0; i < data.size; ++i) {
        vu8_push_back(&cur_block, 1);
        uint8_t cur_num = vu8_get(data, i);
        if (cur_num == 0) {
            vu8_push_back(&cur_block, 1);
            continue;
        }
        int flag = 0;
        for (int j = CNT_FIB - 1; j > 0; --j) {
            if (cur_num >= fib_nums[j]) {
                flag = 1;
                vu8_push_back(&cur_block, 1);
                cur_num -= fib_nums[j];
            } else if (flag) {
                vu8_push_back(&cur_block, 0);
            }
        }
        vu8_push_back(&cur_block, 0);
        add_fibonacci_block(&result, &cur_block, &cur_bit);
    }
    free(cur_block.data);
    return result;
}

void
decompress_fibonacci_block(Vector_uint8 *result, Vector_uint8 *ans) {
    size_t cur_fib_num = 0;
    uint8_t cur_num = 0;
    for (size_t i = ans->size - 1; i > 0; --i) {
        if (vu8_get(*ans, i) == 1) {
            cur_num += fib_nums[cur_fib_num];
        }
        ++cur_fib_num;
    }
    vu8_push_back(result, cur_num);
    while (ans->size > 0) {
        vu8_erase(ans, 0);
    }
}


Vector_uint8
fibonacci_decompress(Vector_uint8 data) {
    Vector_uint8 result = vu8_init(0);
    Vector_uint8 cur_block = vu8_init(0);
    int ones_in_row = 0, flag = 0;
    for (size_t i = 0; i < data.size; ++i) {
        for (int j = BITS_IN_UINT8_T - 1; j >= 0; --j) {
            vu8_push_back(&cur_block, get_bit(vu8_get(data, i), j));
            if (vu8_get(cur_block, cur_block.size - 1) == 1) {
                ones_in_row++;
                if (ones_in_row == 3) {
                    flag = 1;
                    vu8_push_back(&result, 0);
                    ones_in_row = 0;
                    vu8_erase(&cur_block, 0);
                    vu8_erase(&cur_block, 0);
                }
                if (ones_in_row == 2) {
                    if (flag) {
                        vu8_erase(&cur_block, cur_block.size - 1);
                        vu8_erase(&cur_block, cur_block.size - 1);
                        decompress_fibonacci_block(&result, &cur_block);
                        vu8_push_back(&cur_block, 1);
                        vu8_push_back(&cur_block, 1);
                    }
                    flag = 1;
                }
            } else {
                ones_in_row = 0;
            }
        }
    }
    return result;
}

Vector_uint8
compress(void *raw_data, size_t data_len, CompressType mod) { // length is calculated in bytes
    Vector_uint8 result = {0};
    Vector_uint8 data = vu8_init(0);
    for (size_t i = 0; i < data_len; ++i) {
        vu8_push_back(&data, ((uint8_t *)raw_data)[i]);
    }
    if (data.data == NULL) {
        fprintf(stderr, "Invalid input data\n");
        return result;
    }
    switch (mod) {
        case MOCK_COMPRESS:
            result = mock_compress(data);
            break;
        case RLE:
            result = rle_compress(data);
            break;
        case SIMPLE9:
            result = simple9_compress(data);
            break;
        case FIBONACCI:
            result = fibonacci_compress(data);
            break;
        default:
            fprintf(stderr, "Invalid input data\n");
            break;
    }
    return result;
}

Vector_uint8
decompress(void *raw_data, size_t data_len, CompressType mod) {
    Vector_uint8 result = {0};
    Vector_uint8 data = vu8_init(0);
    for (size_t i = 0; i < data_len; ++i) {
        vu8_push_back(&data, ((uint8_t *)raw_data)[i]);
    }
    if (data.data == NULL) {
        return result;
    }
    switch(mod) {
        case MOCK_COMPRESS:
            result = mock_compress(data);
            break;
        case RLE:
            result = rle_decompress(data);
            break;
        case SIMPLE9:
            result = simple9_decompress(data);
            break;
        case FIBONACCI:
            result = fibonacci_decompress(data);
            break;
        default:
            break;
    }
    return result;
}
