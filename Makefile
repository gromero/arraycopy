CC=gcc
CFLAGS=-O0 -g
LDFLAGS=-lm

all: 	arraycopy.o arraycopy_vsx.o arraycopy_lvx.o arraycopy_memcpy.o
	$(CC) $(LDFLAGS) arraycopy.o -o arraycopy
	$(CC) $(LDFLAGS) arraycopy.o -o arraycopy_with_check -DCHECK
	$(CC) $(LDFLAGS) arraycopy_vsx.o -o arraycopy_vsx
	$(CC) $(LDFLAGS) arraycopy_vsx.o -o arraycopy_vsx_with_check -DCHECK
	$(CC) arraycopy_lvx.o -o arraycopy_lvx
	$(CC) arraycopy_lvx.o -o arraycopy_lvx_with_check -DCHECK
	$(CC) arraycopy_memcpy.o -o arraycopy_memcpy
	$(CC) arraycopy_memcpy.o -o arraycopy_memcpy_with_check -DCHECK

test:	all
	./arraycopy_with_check
	./arraycopy_vsx_with_check
	./arraycopy_lvx_with_check
	./arraycopy_memcpy_with_check

clean:
	rm -fr *.o
	rm -fr arraycopy arraycopy_vsx
	rm -fr arraycopy_with_check arraycopy_vsx_with_check
	rm -fr arraycopy_lvx  arraycopy_memcpy
	rm -fr arraycopy_lvx_with_check arraycopy_memcpy_with_check
	rm -fr perf*

