#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "source.h"

#define ELEM_SIZE          8 			        // 8 bytes for uint64_t
#define NUM_ELEM_IN_BUFFER 1024*1024*2   	        // 2 M elements in buffer
#define BUFFER_SIZE  	   NUM_ELEM_IN_BUFFER*ELEM_SIZE // 16 MiB in buffer


void arraycopy(uint64_t *dst, uint64_t *src, size_t n)
{
  size_t i = 0;

  // Copy 16 64-bit elements per loop.
  i = n / 16;

  asm (
       "        cmpldi   %2,  0         \n\t"
       "        beq       2f            \n\t"
       "        li        6, 16	        \n\t"
       "        li        7, 32	        \n\t"
       "        li        8, 48 	\n\t"
       "        mtctr    %2	        \n\t"
       "        .align 4                \n\t"
       "1:      lxvd2x    6,  0, %1 	\n\t"
       "        lxvd2x    7, %1,  6 	\n\t"
       "  	lxvd2x    8, %1,  7 	\n\t"
       "        lxvd2x    9, %1,  8	\n\t"
       "        stxvd2x   6,  0, %0 	\n\t"
       "        addi     %1, %1, 64 	\n\t"
       "        stxvd2x   7, %0,  6 	\n\t"
       " 	stxvd2x   8, %0,  7	\n\t"
       "    	stxvd2x   9, %0,  8	\n\t"
       "        lxvd2x    6,  0, %1 	\n\t"
       "        lxvd2x    7, %1,  6 	\n\t"
       "      	addi     %0, %0, 64  	\n\t"
       "        lxvd2x    8, %1,  7 	\n\t"
       "        lxvd2x    9, %1,  8 	\n\t"
       "        addi     %1, %1, 64 	\n\t"
       "        stxvd2x   6,  0, %0 	\n\t"
       "        stxvd2x   7, %0,  6 	\n\t"
       " 	stxvd2x   8, %0,  7	\n\t"
       "    	stxvd2x   9, %0,  8	\n\t"
       "      	addi     %0, %0, 64  	\n\t"
       "	bdnz+ 	 1b	        \n\t"
       "2:      nop                     \n\t"
        :
        : "r"(dst), "r"(src), "r"(i)
        : "memory", "3", "4", "5", "6", "7", "8"
       );

  for (int j = i*16; j < n; ++j)
    dst[j] = src[j];
}

int main(void)
{
//uint64_t* source      => from source.h, random data.
  uint64_t* destination = (uint64_t*)malloc(BUFFER_SIZE); // 16 MiB, or 2 M 64-bit elements.

  // Waist some time here.
  for (int p = 0; p < 2500; ++p) {
    arraycopy(destination, source, NUM_ELEM_IN_BUFFER);
  }

#if defined(CHECK)
  printf("Verifying if copy is ok...\n");


  for (int p = 0; p < NUM_ELEM_IN_BUFFER; ++p) {
    if (destination[p] != source[p]) {
      printf(">> Mismatch @%d: %#lx != %#lx\n", p,              \
                                                destination[p], \
                                                source[p]);
      exit(1);
    }
  }

  printf("Done.\n");
#endif

  exit(0);
}
