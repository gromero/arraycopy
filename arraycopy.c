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
#define NUM_ELEM_IN_BUFFER 1024*1024*2 		        // 2 M elements in buffer
#define BUFFER_SIZE  	   NUM_ELEM_IN_BUFFER*ELEM_SIZE // 16 MiB in buffer

#if !defined(VSX)
void arraycopy(uint64_t *dst, uint64_t *src, size_t n)
{
  size_t i;
  size_t remainder;

  // Bulk rd/wr size is 4, ie 4 x 8 bytes, or
  // 4 x 64-bit elements rd/wr "at once".
  remainder = n % 4;
  i = n / 4;

#if defined(VERBOSE)
  printf("Copying %ld 64-bit element(s), "  \
            "with %ld block iteration(s) " \
             "and %ld byte(s) as remainder(s)\n", n, i, remainder);
#endif

  asm (
       " 	li 3, 7+8	\n\t"
       "	mtspr 3, 3	\n\t"
       "        cmpldi %2, 0    \n\t"
       "        beq   2f        \n\t"
       "        mtctr %2	\n\t"
       /********* Main Code ********/
       "1:      ld  3, 0(%1)    \n\t"
       "        ld  4, 8(%1)    \n\t"
       "        ld  5,16(%1)    \n\t"
       "        ld  6,24(%1)    \n\t"
       "        std 3, 0(%0)    \n\t"
       "        std 4, 8(%0)    \n\t"
       "        std 5,16(%0)    \n\t"
       "        std 6,24(%0)    \n\t"
       /****************************/
       "        addi %1, %1, 32 \n\t"
       "        addi %0, %0, 32 \n\t"
       "        bdnz+ 1b	\n\t"
       "2:      nop             \n\t"
        :
        : "r"(dst), "r"(src), "r"(i)
        : "memory", "r3", "r4", "r5", "r6"
       );

  for (int j = i*4; j < n; ++j)
    dst[j] = src[j];
}

#else
void arraycopy(uint64_t *dst, uint64_t *src, size_t n)
{
  size_t i = 0;
  size_t remainder = 0;

  // Bulk rd/wr size is 8, ie 8 x 8 bytes, or
  // 8 x 64-bit elements rd/wr "at once".
  remainder = n % 8;
  i = n / 8;

#if defined(VERBOSE)
  printf("VSX copying %ld 64-bit element(s), " \
                "with %ld block iteration(s) " \
                 "and %ld byte(s) as remainder(s)\n", n, i, remainder);
#endif

  asm (
       "        cmpldi   %2,  0         \n\t"
       "        beq       2f            \n\t"
       "        li        6, 16	        \n\t"
       "        li        7, 32	        \n\t"
       "        li        8, 48 	\n\t"
       "        mtctr    %2	        \n\t"
       "1:      lxvd2x    6,  0, %1 	\n\t"
       "        lxvd2x    7, %1,  6 	\n\t"
       "  	lxvd2x    8, %1,  7 	\n\t"
       "        lxvd2x    9, %1,  8	\n\t"
       "        stxvd2x   6,  0, %0 	\n\t"
       "        stxvd2x   7, %0,  6 	\n\t"
       " 	stxvd2x   8, %0,  7	\n\t"
       "    	stxvd2x   9, %0,  8	\n\t"
       "        addi     %1, %1, 64 	\n\t"
       "      	addi     %0, %0, 64  	\n\t"
       "	bdnz+ 	 1b	        \n\t"
       "2:      nop                     \n\t"
        :
        : "r"(dst), "r"(src), "r"(i)
        : "memory", "3", "4", "5", "6", "7", "8"
       );

  for (int j = i*8; j < n; ++j)
    dst[j] = src[j];
}
#endif

int main(void)
{
  printf("** Inline ASM for VSX test **\n\n");

//uint64_t* source      => from source.h
  uint64_t* destination = malloc(BUFFER_SIZE); // 16 MiB, or 2 M 64-bit elements.

  printf("1. Exercising...\n");

  // Waist some time here.
  for (int p = 0; p < 2500; ++p) {

#if defined(MEMCPY) // Use libc memcpy().
    memcpy(destination, source, BUFFER_SIZE);

#else // use our crafted VSX copy.
    arraycopy(destination, source, NUM_ELEM_IN_BUFFER);

#endif
  }

  printf("1. Done.\n");

#if defined(CHECK)
  printf("2. Verifying if copy is ok...\n");


  for (int p = 0; p < NUM_ELEM_IN_BUFFER; ++p) {
    if (destination[p] != source[p]) {
      printf(">> Mismatch @%d: %#lx != %#lx\n", p,              \
                                                destination[p], \
                                                source[p]);
      exit(1);
    }
  }

  printf("2. Done.\n");
#endif

  exit(0);
}
