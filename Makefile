

TARGET=compressor.out decompressor.out
CC=gcc
MAINDIR=compress
SRCDIR=lib/src
BINDIR=lib/bin
HEADDIR=lib/headers
FLAGS= -Wall -iquote $(HEADDIR) -g

all: make_dirs $(TARGET)

make_dirs:
	mkdir -p  $(BINDIR)

compressor.out: compressor.o  lib
	$(CC) $< -L$(BINDIR) -o $@ -lvector_uint8 -lcompress

decompressor.out: decompressor.o lib
	$(CC) $< -L$(BINDIR) -o $@ -lvector_uint8 -lcompress

compressor.o: $(MAINDIR)/compressor.c
	$(CC) -c $< -o $@ $(FLAGS)

decompressor.o: $(MAINDIR)/decompressor.c
	$(CC) -c $< -o $@ $(FLAGS)

libvector_uint8.so: $(HEADDIR)/vector_uint8.h

libvector_uint8.so: $(SRCDIR)/vector_uint8.c
	$(CC) -c --shared $< -o $(BINDIR)/$@ $(FLAGS)

libcompress.so: $(HEADDIR)/compress.h

libcompress.so: $(SRCDIR)/compress.c #compiling dynamic library
	$(CC) -c --shared $< -o $(BINDIR)/$@ $(FLAGS)

lib: libcompress.so libvector_uint8.so

run: all #taget to run code
	./$(TARGET)
clean:
	rm -f *.o

