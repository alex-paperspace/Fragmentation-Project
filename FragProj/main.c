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
	int size;
};

void* pool_allocate(int req_size, Pool* pool) {
	Node* old_head = pool->avail_head;
	Node* current = old_head;
	Node* prev = NULL;
	void* res = NULL;

	while (current != NULL) {
		//difference between this block's size versus the requested size + offset
		const int total_size = req_size + sizeof(int);
		int diff = current->size - total_size;

		if (diff >= 0) {

			//found available chunk
			res = (char*)current + sizeof(int);

			//set offset to req_size
			int* offset = (int*)current;
			*offset = total_size;
			
			//fix avail list
			if (current == old_head) {
				if (diff >= sizeof(Node)) {
					Node* temp = pool->avail_head->next;
					pool->avail_head = (char*)current + total_size;
					pool->avail_head->size = diff;
					pool->avail_head->next = temp;
				}
				else {
					//results in small or 0 fragmentation
					pool->avail_head = pool->avail_head->next;
				}
			}
			else {
				//current addr is after old_head
				if (diff >= sizeof(Node)) {
					//diff is >= size needed to insert a new avail_node
					
					prev->next = (char*)current + total_size;
					Node* newnode = prev->next;
					newnode->size = diff;
					newnode->next = current->next;
				}
				else {
					//results in small or 0 fragmentation
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

	//determine if current not in pool addr range
	if (dptr < pool->memory || dptr > (pool->memory + pool->size)) {
		return NULL;
	}

	Node* newnode = (char*)dptr - sizeof(int);
	int size2free = *(int*)newnode;

	//find spot to insert new Node
	Node* current = pool->avail_head;
	Node* prev = NULL;
	while (current < dptr) {
		prev = current;
		current = current->next;
	}
	/*
	now we have:
	
	|prev|------------->|current|
			 |dptr| somewhere in between the addr gap

	*/
	if (prev != NULL) {
		//insertion is not head of list
		prev->next = newnode;
	}
	else {
		//insertion is head of list
		pool->avail_head = newnode;
	}
	newnode->size = size2free;
	newnode->next = current;
	

	//pool_compact(pool);
}

void pool_compact(Pool* pool) {
	//compacts adjacent available memory locations
	Node* current = pool->avail_head;

	while (current != NULL) {
		while ((char*)current + current->size == current->next) {
			Node* second = current->next;
			int newsize = current->size + second->size;
			current->size = newsize;
			current->next = second->next;
			second->size = NULL;
			second->next = NULL;
		}
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
	pool.size = INIT_SIZE;

	printf("Address of pool avail head: %p\n", pool.avail_head);

	int* ptr = (int*)pool_allocate(4, &pool);
	*ptr = 5;
	printf("Address: %p\n", ptr);
	printf("Value: %d\n", *ptr);

	int* ptr2 = (int*)pool_allocate(4, &pool);
	*ptr2 = 10;
	printf("Address: %p\n", ptr2);
	printf("Value: %d\n", *ptr2);

	pool_free(ptr, &pool);
	pool_free(ptr2, &pool);
	pool_compact(&pool);

	printf("Address of pool avail head: %p\n", pool.avail_head);

	free(pool.memory);
	return 0;
}

