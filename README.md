# Fragmentation-Project

## Two-Size Random Memory Allocator.

Alter pool size, iterations, and sizes at the top of main.c

Freeing is not smart, it returns the memory back to the pool as a node in the avail_head list.
Compacting can be called to combine adjacent unused memory blocks after every free.

#### macros.h

There is already average information about all the iterations, but more can be included...

**#define VERBOSE**

- Information about each iteration

**#define VERYVERBOSE**

- Every allocation/deallocation

**#define COMPACT_AFTER_FREE**

- pool_compact() called after every free to combine adjacent memory blocks (nodes in the avail_head list).
