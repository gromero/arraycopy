all:	arraycopy.c
	gcc -O0 -g arraycopy.c -o arraycopy
	gcc -O0 -g arraycopy.c -o arraycopy_vsx -DVSX

debug:
	gcc -O0 -g arraycopy.c -o arraycopy -DDEBUG
	gcc -O0 -g arraycopy.c -o arraycopy_vsx -DVSX -DDEBUG

check:
	gcc -O0 -g arraycopy.c -o arraycopy -DCHECK
	gcc -O0 -g arraycopy.c -o arraycopy_vsx -DCHECK

clean:
	rm -fr arraycopy arraycopy_vsx
	rm -fr perf*
