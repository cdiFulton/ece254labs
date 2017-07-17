/**
  * ECE254 Linux Dynamic Memory Management Lab
  * @file: main_test.c
  * @brief: The main file to write tests of memory allocation algorithms
  */ 

/* includes */
/* system provided header files. You may add more */
#include <stdio.h>
#include <stdlib.h>

/* non-system provided header files. 
   Do not include more user-defined header files here
 */
#include "mem.h"



int main(int argc, char *argv[])
{
	int best_fit_ext;
	int worst_fit_ext;
	int random;
	int pool_size = 2048;
	int num_iterations = 500;
	int i, j;
	int index = 0;
	void* best_mem[5000];
	void* worst_mem[5000];
	
	printf("check 1\n"); fflush(stdout);
	best_fit_memory_init(pool_size);		// initialize 1KB, best fit
	printf("check 3\n"); fflush(stdout);

	// To test, run through a large number of times allocating then deallocating random
	// numbers of blocks of random size
	for(i = 0; i < num_iterations; i++) {
		random = rand()%5 + 2;
		for(j = 0; j < random; j++) {
			printf("allocate i: %d, random: %d\n", i, random); fflush(stdout);
			best_mem[index] = best_fit_alloc(rand()%pool_size);
			index++;
		}
		
		random = rand()%5;
		for(j = 0; j < random; j++) {
			printf("deallocate i: %d, random: %d\n", i, random); fflush(stdout);
			best_fit_dealloc(best_mem[rand()%index]);
		}
	}
	
	best_fit_ext = best_fit_count_extfrag(pool_size);

	
	printf("check 1\n"); fflush(stdout);
	worst_fit_memory_init(pool_size);		// initialize 1KB, worst fit
	printf("check 3\n"); fflush(stdout);

	for(i = 0; i < num_iterations; i++) {
		random = rand()%5 + 2;
		for(j = 0; j < random; j++) {
			printf("allocate i: %d, random: %d\n", i, random); fflush(stdout);
			worst_mem[index] = worst_fit_alloc(rand()%pool_size);
			index++;
		}
		
		random = rand()%5;
		for(j = 0; j < random; j++) {
			printf("deallocate i: %d, random: %d\n", i, random); fflush(stdout);
			worst_fit_dealloc(worst_mem[rand()%index]);
		}
	}
	
	worst_fit_ext = worst_fit_count_extfrag(pool_size);
	printf("Best fit external fragmentation count: %d\n", best_fit_ext);
	printf("Worst fit external fragmentation count: %d\n", worst_fit_ext);

	return 0;
}
