all:	arraycopy.c arraycopy_vsx.c
	gcc -O0 -g arraycopy.c -o arraycopy
	gcc -O0 -g arraycopy.c -o arraycopy_with_check -DCHECK
	gcc -O0 -g arraycopy_vsx.c -o arraycopy_vsx
	gcc -O0 -g arraycopy_vsx.c -o arraycopy_vsx_with_check -DCHECK

clean:	
	rm -fr arraycopy
	rm -fr arraycopy_with_check
	rm -fr arraycopy_vsx
	rm -fr arraycopy_vsx_with_check
	rm -fr perf*
