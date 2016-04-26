CC=gcc
CFLAGS=-O0 -g

all: 	arraycopy.o arraycopy_vsx.o arraycopy_lvx.o arraycopy_memcpy.o
	$(CC) arraycopy.o -o arraycopy
	$(CC) arraycopy.o -o arraycopy_with_check -DCHECK
	$(CC) arraycopy_vsx.o -o arraycopy_vsx
	$(CC) arraycopy_vsx.o -o arraycopy_vsx_with_check -DCHECK
	$(CC) arraycopy_lvx.o -o arraycopy_lvx
	$(CC) arraycopy_lvx.o -o arraycopy_lvx_with_check -DCHECK
	$(CC) arraycopy_memcpy.o -o arraycopy_memcpy
	$(CC) arraycopy_memcpy.o -o arraycopy_memcpy_with_check -DCHECK

check:
	gcc -O0 -g arraycopy.c -o arraycopy -DCHECK
	gcc -O0 -g arraycopy.c -o arraycopy_vsx -DCHECK

clean:
	rm -fr *.o
	rm -fr arraycopy arraycopy_vsx
	rm -fr arraycopy_with_check arraycopy_vsx_with_check
	rm -fr arraycopy_lvx  arraycopy_memcpy
	rm -fr arraycopy_lvx_with_check arraycopy_memcpy_with_check
	rm -fr perf*
