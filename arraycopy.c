#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

// Number of elements inside buffer and to be
// loaded and stored between two different
// locations in the storage.
#define NUM_OF_ELEM_IN_BUFFER 1 <-----------

// Number of bytes occupied by an element in buffer. Since
// in this particular example we are using uint64_t, an element
// is a 64-bit entity and, as such, its size is 8 bytes.
#define ELEMENT_SIZE 8

// 32 bytes (or 4 64-bit elements) copied
// at a time (per loop - see inline ASM).
#define BULK_COPY_SIZE 32

// Calculate buffer size in bytes able to hold a given
// number of elements.
#define BUFFER_SIZE NUM_OF_ELEM_IN_BUFFER * ELEMENT_SIZE


#if !defined(VSX)
void arraycopy(uint64_t *dst, uint64_t *src, size_t n /* # of 64-bit elements */)
{
  size_t loop;
  size_t remainder;

  loop      = n / (BULK_COPY_SIZE / ELEMENT_SIZE); // n / 4, Four 64-bit elements.
  remainder = n % (BULK_COPY_SIZE / ELEMENT_SIZE); // n % 4, Four 64-bit elements.

#if defined(DEBUG)
  printf("Copying %ld 64-bit element(s), " \
            "with %ld block iteration(s) " \
             "and %ld byte(s) as remainder(s)\n", n, loop, remainder);
#endif

  asm (
       "        cmpldi %2, 0    \n\t" // Check if n < 4
       "        beq- 2f         \n\t" //
       "        mtctr %2        \n\t" // If not, set counter
       /********* Main Code ********/
       "1:      ld  3, 0(%1)    \n\t" // Copy 4 elements.
       "        ld  4, 8(%1)    \n\t"
       "        ld  5,16(%1)    \n\t"
       "        ld  6,24(%1)    \n\t"
       "        std 3, 0(%0)    \n\t"
       "        std 4, 8(%0)    \n\t"
       "        std 5,16(%0)    \n\t"
       "        std 6,24(%0)    \n\t"
       "        addi %1, %1, 32 \n\t" // Update src pointer.
       "        addi %0, %0, 32 \n\t" // Update dst pointer.
       /****************************/
       "        bdnz 1b         \n\t" // Dec. counter, if zero
       "2:      nop             \n\t" // jump here (otherwise loop 1).
        :
        : "r"(dst), "r"(src), "r"(loop)
        : "memory", "r3", "r4", "r5", "r6"
       );

  // Deal with remainers and if n < 4.
  for (int j = loop * 4; j < n; ++j)
    dst[j] = src[j];
}

#else
void arraycopy(uint64_t *dst, uint64_t *src, size_t n /* # of 64-bit elements */)
{
  size_t loop;
  size_t remainder;

  loop      = n / (BULK_COPY_SIZE / ELEMENT_SIZE); // n / 4, Four 64-bit elements.
  remainder = n % (BULK_COPY_SIZE / ELEMENT_SIZE); // n % 4, Four 64-bit elements.

#if defined(DEBUG)
  printf("Copying %ld 64-bit element(s), " \
            "with %ld block iteration(s) " \
             "and %ld byte(s) as remainder(s)\n", n, loop, remainder);
#endif

  asm (
       "        cmpldi %2, 0     \n\t" // Check if n < 4 and
       "        beq- 2f          \n\t" // if so bail out to 3.
       "        li 5, 16         \n\t" // r5 = offset.
       "        mtctr %2         \n\t" // Set counter.
       /********* Main Code ********/
       "1:      lxvd2x  3, 0, %1 \n\t" // Copy 4 elements.
       "        lxvd2x  4, %1, 5 \n\t"
       "        stxvd2x 3, 0, %0 \n\t"
       "        stxvd2x 4, %0, 5 \n\t"
       "        addi %1, %1, 32  \n\t" // Update src
       "        addi %0, %0, 32  \n\t" // Update dst
       /****************************/
       "        bdnz  1b         \n\t" // Dec. counter, if zero
       "2:      nop              \n\t" // jump here (otherwise loop to 1).
        :
        : "r"(dst), "r"(src), "r"(loop)
        : "memory", "r3", "r4",
       );

  // Deal with remainers and if n < 4.
  for (int j = i*4; j < n; ++j)
    dst[j] = src[j];
}
#endif

int main(void)
{
  printf("** Inline ASM for VSX test **\n\n");

  uint64_t* source      = malloc(BUFFER_SIZE); // 16 MiB, or 1 MiB 64-bit element(s).
  uint64_t* destination = malloc(BUFFER_SIZE); // 16 MiB, or 1 MiB 64-bit element(s).

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
  for (int p = 0; p < 5000; ++p) {
    // Copy 16 MiB, or 1 M 64-bit (8 bytes) elements.
    arraycopy(destination, source, NUM_OF_ELEM_IN_BUFFER);
  }

  printf("2. Done.\n");

  printf("3. Verifying if copy is ok...\n");


  for (int p = 0; p < NUM_OF_ELEM_IN_BUFFER; ++p) {
    if (destination[p] != source[p]) {
      printf(">> Mismatch @%d: %#lx != %#lx\n", p,           \
                                                destination, \
                                                source);
      exit(1);
    }
  }

  printf("3. Done.\n");

  exit(0);
}
