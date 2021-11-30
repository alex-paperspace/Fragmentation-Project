#include "randomizer.h"
#include "macros.h"

extern size_1;
extern size_2;

heap_stats random_operations(Pool* pool) {
	int allocations = 0;
	int deallocations = 0;
	int rsrv_size = 0;
	rsrv_node* rsrv_head = NULL;

	int n = 0;
	while (1) {
#ifdef VERYVERBOSE
		printf("%d: ", n);
#endif
		if (rand() % 2 == 0) {
			//allocate
			void* temp;
			if (rand() % 2 == 0) {
				//allocate size_1
				temp = pool_allocate(size_1, pool);
#ifdef VERYVERBOSE
				printf("Allocating %d...\n", size_1);
#endif
			}
			else {
				//allocate size_2
				temp = pool_allocate(size_2, pool);
#ifdef VERYVERBOSE
				printf("Allocating %d...\n", size_2);
#endif
			}

			if (temp == NULL) {
				heap_stats stats = {
					.allocations = allocations,
					.deallocations = deallocations,
					.remaining = pool_remaining(pool),
				};
#ifdef VERBOSE
				printf("No more allocations possible.\nAllocations: %d\nDeallocations: %d\nRemaining fragmented memory: %d bytes / %d bytes\n\n", stats.allocations, stats.deallocations, stats.remaining, pool->size);
#endif
				return stats;
			}
			allocations++;

			rsrv_size++;
			rsrv_node* rsrv_temp = (rsrv_node*)malloc(sizeof(rsrv_node));
			rsrv_temp->addr = temp;
			rsrv_temp->next = rsrv_head;
			rsrv_head = rsrv_temp;

		}
		else {
			//deallocate

			if (rsrv_head == NULL) {
#ifdef VERYVERBOSE
				printf("Nothing to deallocate, continuing...\n");
#endif
				n++;
				continue;
			}

			int list_index = rand() % rsrv_size;
			rsrv_node* temp = rsrv_head;
			rsrv_node* prev = NULL;

			for (int i = 0; i < list_index; ++i) {
				prev = temp;
				temp = temp->next;
			}
#ifdef VERYVERBOSE
			printf("Deallocating ");
#endif
			pool_free(temp->addr, pool);


			if (temp != rsrv_head) {
				prev->next = temp->next;
			}
			else {
				rsrv_head = rsrv_head->next;
			}

			free(temp);

			deallocations++;
			rsrv_size--;
		}
		n++;
	}

}