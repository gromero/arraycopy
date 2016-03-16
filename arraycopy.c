#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

// 16 MiB, or 1 M 64-bit elements
#define CHUNK_SIZE 16*1024*1024

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
       "        cmpldi %2, 0    \n\t"
       "        beq- 2f         \n\t"
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
       "        bdnz 1b		\n\t"
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

  // Bulk rd/wr size is 4, ie 4 x 8 bytes, or
  // 4 x 64-bit elements rd/wr "at once".
  remainder = n % 4;
  i = n / 4;

  printf("VSX copying %ld 64-bit element(s), " \
                "with %ld block iteration(s) " \
                 "and %ld byte(s) as remainder(s)\n", n, i, remainder);

  asm (
       "1:      cmpldi %2, 0     \n\t"
       "        beq- 1f          \n\t"
       "        li 5, 16	 \n\t"
       "        mtctr  %2	 \n\t"
       /********* Main Code ********/
       "2:      lxvd2x  3, 0, %1 \n\t"
       "        lxvd2x  4, %1, 5 \n\t"
       "        addi %1, %1, 16  \n\t"
       "        stxvd2x 3, 0, %0 \n\t"
       "        stxvd2x 4, %0, 5 \n\t"
       "      	addi %0, %0, 16  \n\t"
       /****************************/
       "	bdnz  2b	 \n\t"
//     "        subi %2, %2, 1   \n\t"
//     "        cmpldi %2, 0     \n\t"
//     "        bgt+ 2b          \n\t"
       "1:      nop              \n\t"
        :
        : "r"(dst), "r"(src), "r"(i)
        : "memory", "3", "4", "5"
       );

  for (int j = i*4; j < n; ++j)
    dst[j] = src[j];
}
#endif

int main(void)
{
  printf("** Inline ASM for VSX test **\n\n");

  uint64_t* source      = malloc(CHUNK_SIZE); // 16 MiB, or 1 MiB 64-bit element(s).
  uint64_t* destination = malloc(CHUNK_SIZE); // 16 MiB, or 1 MiB 64-bit element(s).
  uint64_t* zero	= malloc(CHUNK_SIZE); // To hold zeros.

  bzero(zero, CHUNK_SIZE);

  int fd, rc;

  printf("1. Creating source buffer with random data... \n");

  fd = open("/dev/urandom", O_RDONLY);
  if (fd < 0) {
    perror("open:");
    exit(1);
  }

  rc = read(fd, (void*) source, CHUNK_SIZE);
  if (rc != CHUNK_SIZE) {
    perror("read:");
    exit(1);
  } else
    printf("1. Done.\n");

  printf("2. Exercising...\n");

  // Spend some time here.
  for (int p; p < 2500; ++p) {
    // Copy 16 MiB, or 1 M 64-bit (8 bytes) elements.
    arraycopy(destination, source, 1024*1024);
    // Just zero destination.
    arraycopy(destination, zero,   1024*1024);
  }

  printf("2. Done.\n");

  exit(0);
}
