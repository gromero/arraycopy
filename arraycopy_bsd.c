#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "source.h"
#include "bsd_memcpy.h"

#define ELEM_SIZE          8 			        // 8 bytes for uint64_t
#define NUM_ELEM_IN_BUFFER 1024*1024*2   	        // 2 M elements in buffer
#define BUFFER_SIZE  	   NUM_ELEM_IN_BUFFER*ELEM_SIZE // 16 MiB in buffer


int main(void)
{
//uint64_t* source      => from source.h, random data.
  uint64_t* destination = (uint64_t*)malloc(BUFFER_SIZE); // 16 MiB, or 2 M 64-bit elements.

 // copy many times to avoid noise in the comparison
 // use bcopy.S from FreeBSD
 for (int p = 0; p < 2500; ++p) {
    bsd_memcpy(destination, source, BUFFER_SIZE);
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
