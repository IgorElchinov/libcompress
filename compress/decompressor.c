#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compress.h"
#include "vector_uint8.h"

enum {
    MAXN = 100000,
    NUM_OF_PARAMS = 4,
    MAX_FILENAME_LEN = 261,
};


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

int
main(int argc, char **argv) {

    CompressType compress_type = MOCK_COMPRESS;
    
    if (argc != NUM_OF_PARAMS) {
        printf("Wrong list of parameters.\n");
        printf("Use ./decompressor.out <source file name> <destination file name> <compression type>\n");
        print_correct_compression_types_message(stdout);
        fflush(stdout);
        return 1;
    }

    if (cmp_strings(argv[3], "mock\0") || cmp_strings(argv[3], "copy\0")) {
        compress_type = MOCK_COMPRESS;
    } else if (cmp_strings(argv[3], "rle\0")) {
        compress_type = RLE;
    } else if (cmp_strings(argv[3], "simple9\0") || cmp_strings(argv[3], "s9\0")) {
        compress_type = SIMPLE9;
    } else if (cmp_strings(argv[3], "fibonacci\0") || cmp_strings(argv[3], "fib\0")) {
        compress_type = FIBONACCI;
    } else {
        fprintf(stderr, "Incorrect compression type\n");
        print_correct_compression_types_message(stderr);
        fflush(stderr);
        return 1;
    }

    char src_file[MAX_FILENAME_LEN] = "text.txt";
    {
    size_t filename_len = strlen(argv[1]);
    if (filename_len > MAX_FILENAME_LEN) {
        fprintf(stderr, "Too big source filename\n");
        return 1;
    }
    memcpy(src_file, argv[1], filename_len);	
    }

    char dest_file[MAX_FILENAME_LEN] = "text.dec";
    {
    size_t filename_len = strlen(argv[2]);
    if (filename_len > MAX_FILENAME_LEN) {
        fprintf(stderr, "Too big destination filename\n");
        return 1;
    }
    memcpy(dest_file, argv[2], filename_len);	
    }

    FILE *in, *out;
    in = fopen(src_file, "r");
    int n = 0;
    char c;
    while ((c = getc(in)) != EOF) {
        ++n;
    }
    if (n > MAXN) {
        fprintf(stderr, "Too big file\n");
        fclose(in);
        return 1;
    }
    rewind(in);
    char *data;
    data = calloc(n, sizeof(*data));
    for (int i = 0; i < n; ++i) {
        fscanf(in, "%c", &data[i]);
    }
    fclose(in);
    Vector_uint8 decompressed;
    decompressed = decompress(data, n, compress_type);
    if (decompressed.data == NULL) {
        free(data);
        return 1;
    }
    out = fopen(dest_file, "w");
    for (int i = 0; i < decompressed.size; ++i) {
        fprintf(out, "%c", vu8_get(decompressed, i));
    }
    free(data);
    free(decompressed.data);
    fclose(out);
    return 0;
}
