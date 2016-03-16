all:
	gcc -O0 -g arraycopy.c -o arraycopy
	gcc -O0 -g arraycopy.c -o arraycopy_vsx -DVSX

debug:
	gcc -O0 -g arraycopy.c -o arraycopy -DDEBUG
	gcc -O0 -g arraycopy.c -o arraycopy_vsx -DVSX -DDEBUG

clean:
	rm -fr arraycopy arraycopy_vsx
