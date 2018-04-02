CC=gcc
CFLAGS=-O0 -g
LDFLAGS=-lm
ASFLAGS=-O0 -g

all: 	arraycopy_vsx.o arraycopy_bsd.o bcopy.o
	$(CC) $(LDFLAGS) arraycopy_vsx.o bcopy.o -o arraycopy_vsx
	$(CC) $(LDFLAGS) arraycopy_vsx.o bcopy.o -o arraycopy_vsx_with_check -DCHECK
	$(CC) $(LDFLAGS) arraycopy_bsd.o bcopy.o -o arraycopy_bsd
	$(CC) $(LDFLAGS) arraycopy_bsd.o bcopy.o -o arraycopy_bsd_with_check -DCHECK

test:	all
	time ./arraycopy_vsx
	time ./arraycopy_bsd

clean:
	rm -fr *.o
	rm -fr arraycopy_bsd arraycopy_vsx
	rm -fr arraycopy_bsd_with_check arraycopy_vsx_with_check
	rm -fr perf*
