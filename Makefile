all:	arraycopy.c
	gcc -O0 -g arraycopy.c -o arraycopy
	gcc -O0 -g arraycopy.c -o arraycopy_with_check -DCHECK
	gcc -O0 -g arraycopy.c -o arraycopy_vsx -DVSX
	gcc -O0 -g arraycopy.c -o arraycopy_vsx_with_check -DVSX -DCHECK

debug:
	gcc -O0 -g arraycopy.c -o arraycopy -DDEBUG
	gcc -O0 -g arraycopy.c -o arraycopy_vsx -DVSX -DDEBUG

check:
	gcc -O0 -g arraycopy.c -o arraycopy -DCHECK
	gcc -O0 -g arraycopy.c -o arraycopy_vsx -DCHECK

clean:
	rm -fr arraycopy
	rm -fr arraycopy_with_check
	rm -fr arraycopy_vsx
	rm -fr arraycopy_vsx_with_check
	rm -fr perf*
