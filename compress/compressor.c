#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compress.h"
#include "vector_uint8.h"

enum {
    MAXN = 100000,
    MAX_FILENAME_LEN = 256,
    MAX_EXT_LEN = 5
};

const char MOCK_EXT[] = "\0";
const char RLE_EXT[] = ".rle\0";
const char SIMPLE9_EXT[] = ".s9\0";
const char FIBONACCI_EXT[] = ".fib\0";

int
cmp_strings(char *a, char *b) {
    for (int i = 0; i < MAX_FILENAME_LEN; ++i) {
        if (a[i] == '\0' && b[i] == '\0') {
            return 1;
        }
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 0;
}

void
print_correct_compression_types_message(FILE *where) {
    fprintf(where, "Correct compression types are:\n");
    fprintf(where, "- No compression (use 'mock' or 'copy' as 3rd option)\n");
    fprintf(where, "- Run-length encoding (use 'rle' as 3rd option)\n");
    fprintf(where, "- Simple9 comression (use 'simple9' or 's9' as 3rd option)\n");
    fprintf(where, "- Fibonacci compression (use 'fibonacci' or 'fib' as 3rd option)\n");
    fflush(where);
    return;
}

size_t get_file_len(FILE *file) {
    size_t pos = ftell(file);
    rewind(file);
    size_t len = 0;
    char c = 0;
    while ((c = getc(file)) != EOF) {
        ++len;
    }
    fseek(file, pos, SEEK_SET);
    return len;
}

int
main(int argc, char *argv[]) {

    CompressType compress_type = MOCK_COMPRESS;
    char dest_ext[MAX_EXT_LEN];
    if (argc > 2) {
        if (cmp_strings(argv[2], "mock\0") || cmp_strings(argv[2], "copy\0")) {
	        memcpy(dest_ext, MOCK_EXT, MAX_EXT_LEN);
	        compress_type = MOCK_COMPRESS;
        } else if (cmp_strings(argv[2], "rle\0")) {
	        memcpy(dest_ext, RLE_EXT, MAX_EXT_LEN);
	        compress_type = RLE;
	    } else if (cmp_strings(argv[2], "simple9\0") || cmp_strings(argv[2], "s9\0")) {
	        memcpy(dest_ext, SIMPLE9_EXT, MAX_EXT_LEN);
	        compress_type = SIMPLE9;
	    } else if (cmp_strings(argv[2], "fibonacci\0") || cmp_strings(argv[2], "fib\0")) {
	        memcpy(dest_ext, FIBONACCI_EXT, MAX_EXT_LEN);
            compress_type = FIBONACCI;
        } else {
	        fprintf(stderr, "Incorect compress type\n");
            print_correct_compression_types_message(stderr);
            fflush(stderr);
            return 1;
        }
    }
    char src_file[MAX_FILENAME_LEN] = "text.txt";
    char dest_file[MAX_FILENAME_LEN + MAX_EXT_LEN] = "text.txt";
    if (argc > 1) {
        size_t filename_len = strlen(argv[1]);
        if (filename_len > MAX_FILENAME_LEN) {
            fprintf(stderr, "Too big filename\n");
            return 1;
        }
        memcpy(src_file, argv[1], filename_len);
        memcpy(dest_file, argv[1], filename_len);
        strcat(dest_file, dest_ext);	
    }

    
    FILE *in = fopen(src_file, "r");
    int n = get_file_len(in);
    if (n > MAXN) {
        fprintf(stderr, "Too big file\n");
        fclose(in);
        return 1;
    }
    char *data;
    data = calloc(n, sizeof(*data));
    for (int i = 0; i < n; ++i) {
        fscanf(in, "%c", &data[i]);
    }
    fclose(in);
    in = NULL;

    Vector_uint8 compressed;
    compressed = compress(data, n, compress_type);
    free(data);
    data = NULL;
    if (compressed.data == NULL) {
        return 1;
    }
    FILE *out = fopen(dest_file, "w");
    for (int i = 0; i < compressed.size; ++i) {
        fprintf(out, "%c", vu8_get(compressed, i));
    }
    free(compressed.data);
    fclose(out);
    
    return 0;
}
