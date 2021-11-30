#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pool.h"
#include "randomizer.h"

#define KILO (1000)
#define MEGA (1000000)
#define K (1024)
#define M (1048576)


const int gc_pool_size = 50 * KILO;
const int gc_iterations = 50;
const int size_1 = 100;
const int size_2 = 1000;


double g_average_allocs = 0;
double g_average_deallocs = 0;
double g_average_remaining = 0;

void one_pool_iteration() {

	Pool pool;
#ifdef VERBOSE
	printf("Allocating memory pool of %d bytes...\n", gc_pool_size);
#endif VERBOSE
	pool.memory = (char*)malloc(gc_pool_size);
	pool.avail_head = (Node*)pool.memory;
	pool.avail_head->size = gc_pool_size;
	pool.avail_head->next = NULL;
	pool.size = gc_pool_size;

#ifdef VERBOSE
	printf("Random allocator/deallocator starting...\n");
#endif VERBOSE
	heap_stats stats = random_operations(&pool);
	g_average_allocs += stats.allocations;
	g_average_deallocs += stats.deallocations;
	g_average_remaining += stats.remaining;

	free(pool.memory);
}

int main()
{
	srand(time(NULL)); //seed rand

	printf("Starting Random Heap Allocator/Deallocator...\nSettings -\n\tTotal Iterations: %d\n\tPool size: %d bytes\n\tSizes allocated: %d and %d bytes\n\n", gc_iterations, gc_pool_size, size_1, size_2);
	
	for (int i = 1; i <= gc_iterations; ++i) {
#ifdef VERBOSE
		printf("%d:\n", i);
#endif
		one_pool_iteration();
	}

	g_average_allocs /= gc_iterations;
	g_average_deallocs /= gc_iterations;
	g_average_remaining /= gc_iterations;

	printf("Average Allocations: %.2f\nAverage Deallocations: %.2f\nAverage Remaining: %.2f bytes / %d bytes\n", g_average_allocs, g_average_deallocs, g_average_remaining, gc_pool_size);

	return 0;
}

