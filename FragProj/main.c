#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define K (1024)
#define M (1048576)

typedef struct Node Node;
typedef struct Pool Pool;

struct Node {
	int size;
	Node* next;
};

struct Pool {
	char *memory;
	Node* avail_head;
	Node* rsrv_head;
};

void* pool_allocate(int req_size, Pool* pool) {
	Node* old_head = pool->avail_head;
	Node* current = old_head;
	Node* prev = NULL;
	void* res = NULL;

	while (current != NULL) {
		//difference between this block's size versus the requested size
		int diff = current->size - req_size;

		if (diff >= 0) {

			//found available chunk
			res = (void*)current;

			//fix reserve list
			//maybe this should be a FIFO structure instead of a list?
			if (pool->rsrv_head == NULL) {
				//first alloc
				pool->rsrv_head = current;
				Node* newnode = pool->rsrv_head;
				newnode->size = req_size;
				newnode->next = NULL;
			}
			else {
				//not first alloc
				Node* newnode = current;
				newnode->size = req_size;
				newnode->next = pool->rsrv_head;
				pool->rsrv_head = newnode;
			}

			//fix avail list
			if (current == old_head) {
				pool->avail_head = (char*)current + req_size;
				pool->avail_head->size = diff;
			}
			else {
				//current addr is after old_head
				if (diff >= sizeof(Node)) {
					//diff is >= size needed to insert a new avail_node
					
					prev->next = (char*)current + req_size;
					Node* newnode = prev->next;
					newnode->size = diff;
					newnode->next = current->next;
				}
				else {
					/*
					not enough space left to construct a new avail_node
					two possible situations:
					- A very small amount of memory is left with size < 8 and becomes unusable, remaining in a fragmented state
					- Space left is actually 0 and we're just removing this avail_node from the list
					*/
					prev->next = current->next;
				}
			}
			return res;
		}
		//set to next avail block
		prev = current;
		current = current->next;
	}
	return res;
}

void pool_free(void* dptr, Pool* pool) {
	if (dptr == pool->rsrv_head) {
		pool->rsrv_head = pool->rsrv_head->next;
		return;
	}

	Node* current = pool->rsrv_head->next;
	while (current != NULL) {
		current = current->next;
	}
}

int main()
{
	srand(time(NULL)); //seed rand
	
	const int INIT_SIZE = 10 * K;

	Pool pool;

	pool.memory = (char*)malloc(INIT_SIZE);
	pool.avail_head = (Node*)pool.memory;
	pool.avail_head->size = INIT_SIZE;
	pool.avail_head->next = NULL;
	pool.rsrv_head = NULL;

	printf("Address of pool avail head: %p\n", pool.avail_head);

	int* ptr = (int*)pool_allocate(4, &pool);
	*ptr = 5;
	printf("Address: %p\n", ptr);
	printf("Value: %d\n", *ptr);

	pool_free(ptr, &pool);

	/*int* ptr2 = (int*)pool_allocate(4, &pool);
	*ptr2 = 10;
	printf("Address: %p\n", ptr2);
	printf("Value: %d\n", *ptr2);*/

	printf("Address of pool avail head: %p\n", pool.avail_head);

	free(pool.memory);
	return 0;
}

