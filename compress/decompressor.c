#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compress.h"
#include "vector_uint8.h"

enum {
    MAXN = 100000,
    NUM_OF_PARAMS = 3,
    MAX_FILENAME_LEN = 256,
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


int
main(int argc, char **argv) {

    CompressType compress_type = MOCK_COMPRESS;
    
    if (argc != NUM_OF_PARAMS) {
        printf("Wrong list of parameters.\n");
        printf("Use ./decompressor.out <filename> <compress type>\n");
        fflush(stdout);
        return 1;
    }

    if (cmp_strings(argv[2], "mock\0") || cmp_strings(argv[2], "copy\0")) {
        compress_type = MOCK_COMPRESS;
    } else if (cmp_strings(argv[2], "rle\0")) {
        compress_type = RLE;
    } else if (cmp_strings(argv[2], "simple9\0") || cmp_strings(argv[2], "s9\0")) {
        compress_type = SIMPLE9;
    } else if (cmp_strings(argv[2], "fibonacci\0") || cmp_strings(argv[2], "fib\0")) {
        compress_type = FIBONACCI;
    } else {
        fprintf(stderr, "Incorect compress type\n");
    }




    FILE *in, *out;
    in = fopen("text.txt.s9", "r");
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
    Vector_uint8 decompressed;
    decompressed = decompress(data, n, SIMPLE9);
    if (decompressed.data == NULL) {
        fclose(in);
        free(data);
        return 1;
    }
    out = fopen("text.decompressed", "w");
    for (int i = 0; i < decompressed.size; ++i) {
        fprintf(out, "%c", vu8_get(decompressed, i));
    }
    free(data);
    free(decompressed.data);
    fclose(in);
    fclose(out);
    return 0;
}
