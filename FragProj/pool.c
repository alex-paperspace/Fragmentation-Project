#include "pool.h"
#include "macros.h"

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
					pool->avail_head = (Node*)((char*)current + total_size);
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

					prev->next = (Node*)((char*)current + total_size);
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
	if (dptr < (void*)pool->memory || dptr >(void*)(pool->memory + pool->size)) {
		return;
	}

	Node* newnode = (Node*)((char*)dptr - sizeof(int));
	int size2free = *(int*)newnode;
#ifdef VERYVERBOSE
	printf("%d...\n", size2free - sizeof(int));
#endif

	//find spot to insert new Node
	Node* current = pool->avail_head;
	Node* prev = NULL;
	while ((void*)current < dptr) {
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

#ifdef COMPACT_AFTER_FREE
	pool_compact(pool);
#endif
}

void pool_compact(Pool* pool) {
	//compacts adjacent available memory locations, can be called after every pool_free.
	Node* current = pool->avail_head;

	while (current != NULL) {
		while ((Node*)((char*)current + current->size) == current->next) {
			Node* second = current->next;
			int newsize = current->size + second->size;
			current->size = newsize;
			current->next = second->next;
			second->size = 0;
			second->next = NULL;
		}
		current = current->next;
	}
}

int pool_remaining(Pool* pool) {
	Node* current = pool->avail_head;
	int remaining_mem = 0;
	while (current != NULL) {
		remaining_mem += current->size;
		current = current->next;
	}
	return remaining_mem;
}