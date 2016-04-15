all:
	gcc -O0 -g arraycopy.c -o arraycopy
	gcc -O0 -g arraycopy.c -o arraycopy_with_check -DCHECK
	gcc -O0 -g arraycopy_vsx.c -o arraycopy_vsx
	gcc -O0 -g arraycopy_vsx.c -o arraycopy_vsx_with_chech -DCHECK
	gcc -O0 -g arraycopy_lvx.c -o arraycopy_lvx
	gcc -O0 -g arraycopy_lvx.c -o arraycopy_lvx_with_check -DCHECK
	gcc -O0 -g arraycopy_memcpy.c -o arraycopy_memcpy
	gcc -O0 -g arraycopy_memcpy.c -o arraycopy_memcpy_with_check -DCHECK

check:
	gcc -O0 -g arraycopy.c -o arraycopy -DCHECK
	gcc -O0 -g arraycopy.c -o arraycopy_vsx -DCHECK

clean:
	rm -fr arraycopy arraycopy_vsx
	rm -fr arraycopy_with_check arraycopy_vsx_with_check
	rm -fr arraycopy_lvx  arraycopy_memcpy
	rm -fr arraycopy_lvx_with_check arraycopy_memcpy_with_check
	rm -fr perf*
