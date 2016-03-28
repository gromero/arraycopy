#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define ELEM_SIZE 8 // 8 bytes

#define NUM_ELEM_IN_BUFFER 1024*1024*2 // 2M elements in buffer

#define BUFFER_SIZE  NUM_ELEM_IN_BUFFER*ELEM_SIZE // 16 MiB in buffer

#if !defined(VSX)
void arraycopy(uint64_t *dst, uint64_t *src, size_t n)
{
  size_t i;
  size_t remainder;

  // Bulk rd/wr size is 4, ie 4 x 8 bytes, or
  // 4 x 64-bit elements rd/wr "at once".
  remainder = n % 4;
  i = n / 4;

  printf("Copying %ld 64-bit element(s), "  \
            "with %ld block iteration(s) " \
             "and %ld byte(s) as remainder(s)\n", n, i, remainder);
  asm (
       " 	li 3, 7+8	\n\t"
       "	mtspr 3, 3	\n\t"
//     "slow:	lis 4, slow@ha	\n\t"
//     "	addi 4,4,slow@l \n\t"
//     "        xor 3,3,3       \n\t"
//     "	icbt 2,3,4	\n\t"
       "        cmpldi %2, 0    \n\t"
       "        beq   2f        \n\t"
       "        mtctr %2	\n\t"
       /********* Main Code ********/
       "1:      ld  3, 0(%1)    \n\t"
       "        ld  4, 8(%1)    \n\t"
       "        ld  5,16(%1)    \n\t"
//     "	dcbz 0,%1	\n\t"
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

  printf("VSX copying %ld 64-bit element(s), " \
                "with %ld block iteration(s) " \
                 "and %ld byte(s) as remainder(s)\n", n, i, remainder);
  asm (
       ".align 4                        \n\t"
       " 	li    %%r9,   7         \n\t"
       "	mtspr   3, %%r9         \n\t" // Set data stream to deepest pre-fetch.
       "        cmpldi %2, 0            \n\t"
       "        beq    exitt            \n\t"
       "        li    %%r5, 16	        \n\t"
       "        li    %%r6, 32	        \n\t"
       "        li    %%r7, 48 	        \n\t"
       "        mtctr  %2	        \n\t"
//     " 	lis  %%r9, bulk@ha      \n\t"
//     "	addi %%r9, 9, bulk@l    \n\t"
//     "	icbt	0, 0, 9         \n\t" // Touch code @2f to L1 cache
//     " 	lis  %%r9, exit@ha      \n\t"
//     "	addi %%r9, 9, exit@l    \n\t"
//     " 	icbt 	0, 0, 9         \n\t" // Touch code @3f to L1 cache
       "bulkk:  lxvd2x  %%v4,  0,  %1 	\n\t" // Load 16 bytes, 2 elem.
       "        lxvd2x  %%v5, %1, %%r5 	\n\t" // plus 2 elem.
       "  	lxvd2x  %%v6, %1, %%r6 	\n\t" // plus 2 elem.
       "        lxvd2x  %%v7, %1, %%r7	\n\t" // and finally we got 8 elems.
       "        addi     %1, %1,  64 	\n\t" // Update src by 64 bytes (8 elems).
       "        stxvd2x %%v4,  0,  %0 	\n\t" // Store them all back.
       "        stxvd2x %%v5, %0, %%r5 	\n\t"
       " 	stxvd2x %%v6, %0, %%r6	\n\t"
       "    	stxvd2x %%v7, %0, %%r7	\n\t"
       "      	addi     %0, %0, 64  	\n\t" // Update dst by 64 bytes (8 elemt).
       "	bdnz+  bulkk	        \n\t"
       "exitt:   nop                    \n\t"
        :
        : "r"(dst), "r"(src), "r"(i)
        : "memory", "3", "4", "5", "6", "7"
       );

  for (int j = i*8; j < n; ++j)
    dst[j] = src[j];
}
#endif

int main(void)
{
  printf("** Inline ASM for VSX test **\n\n");

  uint64_t* source      = malloc(BUFFER_SIZE); // 16 MiB, or 2 M 64-bit elements.
  uint64_t* destination = malloc(BUFFER_SIZE); // 16 MiB, or 2 M 64-bit elements.

  int fd, rc;

  printf("1. Creating source buffer with random data... \n");

  fd = open("/dev/urandom", O_RDONLY);
  if (fd < 0) {
    perror("open:");
    exit(1);
  }

  rc = read(fd, (void*) source, BUFFER_SIZE);
  if (rc != BUFFER_SIZE) {
    perror("read:");
    exit(1);
  } else
    printf("1. Done.\n");

  printf("2. Exercising...\n");

  // Spend some time here.
  for (int p = 0; p < 2500; ++p) {
#if defined(MEMCPY) // Use glib memcpy().
    memcpy(destination, source, BUFFER_SIZE);
#else // Use our crafted VSX copy.
    arraycopy(destination, source, NUM_ELEM_IN_BUFFER);
#endif
  }

  printf("2. Done.\n");

  exit(0);
}
