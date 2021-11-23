#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define K (1024)
#define M (1048576)

typedef struct avail_node avail_node;
typedef struct Pool Pool;

struct avail_node {
	int size;
	avail_node* next;
};

struct Pool {
	char *memory;
	avail_node* avail_head;
};

void pool_insert_avail_node(int diff) {

}

void* pool_allocate(int req_size, Pool* pool) {
	avail_node* old_head = pool->avail_head;
	avail_node* current = old_head;
	avail_node* prev = NULL;
	void* res = NULL;

	while (current != NULL) {
		//difference between this block's size versus the requested size
		int diff = current->size - req_size;

		if (diff >= 0) {

			//found available chunk
			res = (void*)current;

			//fix avail list
			if (current == old_head) {
				pool->avail_head = (char*)current + req_size;
				pool->avail_head->size = diff;
			}
			else {
				//current addr is after old_head
				if (diff >= sizeof(avail_node)) {
					//diff is >= size needed to insert a new avail_node
					
					prev->next = (char*)current + req_size;


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

int main()
{
	srand(time(NULL)); //seed rand
	
	const int INIT_SIZE = 10 * K;

	Pool pool;

	pool.memory = (char*)malloc(INIT_SIZE);
	pool.avail_head = (avail_node*)pool.memory;
	pool.avail_head->size = INIT_SIZE;
	pool.avail_head->next = NULL;

	printf("Address of pool avail head: %p\n", pool.avail_head);

	int* ptr = (int*)pool_allocate(4, &pool);
	*ptr = 5;
	printf("Address: %p\n", ptr);
	printf("Value: %d\n", *ptr);

	int* ptr2 = (int*)pool_allocate(4, &pool);
	*ptr2 = 10;
	printf("Address: %p\n", ptr2);
	printf("Value: %d\n", *ptr2);

	printf("Address of pool avail head: %p\n", pool.avail_head);

	free(pool.memory);
	return 0;
}

