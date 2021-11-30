#ifndef POOL_H
#define POOL_H

#include <stdlib.h>

typedef struct Node Node;
typedef struct Pool Pool;

struct Node {
	int size;
	Node* next;
};

struct Pool {
	char *memory;
	Node* avail_head;
	int size;
};

void* pool_allocate(int req_size, Pool* pool);

void pool_free(void* dptr, Pool* pool);

void pool_compact(Pool* pool);

int pool_remaining(Pool* pool);

#endif //POOL_H


