#ifndef MEM_ALLOC
#include <stddef.h>
#define MEM_ALLOC
#define MIN_ALLOCATION_SIZE 4096
void * mem_alloc(size_t to_alloc);
void free_mem(void *block);
#endif