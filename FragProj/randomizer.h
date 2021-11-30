#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include "pool.h"

typedef struct heap_stats heap_stats;
typedef struct rsrv_node rsrv_node;

struct heap_stats {
	int allocations;
	int deallocations;
	int remaining;
};

struct rsrv_node {
	void* addr;
	rsrv_node* next;
};

heap_stats random_operations(Pool* pool);

#endif //RANDOMIZER_H
