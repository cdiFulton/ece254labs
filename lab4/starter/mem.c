/**
 * @file memory.c
 * @brief: ECE254 Lab: memory allocation algorithm comparison template file
 * @author: 
 * @date: 2015-11-20
 */

/* includes */
#include <stdio.h> 
#include <stdlib.h> 
#include "mem.h"
#include "helpers.c"

/* defines */


/* global varaiables */
void* memory_best;
void* memory_worst;

struct mem_ll* ll_best;
struct mem_ll* ll_worst;

const int ll_struct_size = sizeof(struct mem_ll*) + sizeof(struct mem_ll*) + sizeof(size_t) + sizeof(void*) + sizeof(int);

/* Functions */

/* memory initializer */
int best_fit_memory_init(size_t size)
{	
	struct mem_ll* temp = malloc(sizeof(struct mem_ll));
	int init_size = size;
	
	if(size%4) { 
		size += 4 - size%4; 
	}
	
	if( size < ll_struct_size ) {
		return 0;
	}
	
	// Allocating the overall memory pool
	// The first ll_struct_size bytes will be taken up by the ll heads
	memory_best = malloc( init_size );
	
	temp->next = NULL;
	temp->prev = NULL;
	temp->is_allocated = 0;
	temp->size = init_size - ll_struct_size;
	temp->space = memory_best + ll_struct_size;
	
	ll_best = temp;
	
	return 0;
}

int worst_fit_memory_init(size_t size)
{
	struct mem_ll* temp = malloc(sizeof(struct mem_ll));
	int init_size = size;
	
	if(size%4) { 
		size += 4 - size%4; 
	}
	
	if( size < ll_struct_size ) {
		return 0;
	}
	
	// Allocating the overall memory pool
	// The first ll_struct_size bytes will be taken up by the ll heads
	memory_worst = malloc( init_size );
	
	temp->next = NULL;
	temp->prev = NULL;
	temp->is_allocated = 0;
	temp->size = init_size - ll_struct_size;
	temp->space = memory_best + ll_struct_size;
	
	ll_worst = temp;
	
	return 0;
}

/* memory allocators */
void *best_fit_alloc(size_t size)
{
	struct mem_ll* temp_old;
	struct mem_ll* temp_new = malloc(sizeof(struct mem_ll));
	
	if(size%4) { 
		size += 4 - size%4; 
	}
	
	// Look through the list to try to find a suitable block
	temp_old = ll_find_min_block( ll_best, size + ll_struct_size );
	
	// If we did not find an adequate block
	if(temp_old == 0) {
		return NULL;
	}
	printf("best: found element of size %d at %d\n", temp_old->size, (int)temp_old->space); fflush(stdout); 
	
	// Break the found memory block into two blocks: the original found block
	// will now be allocated and of the specified size, and the remaining space will 
	// be unallocated and with the remaining size
	
	// Create the new block after the original block
	temp_new->size = temp_old->size - size - ll_struct_size;
	temp_new->space = temp_old->space + size + ll_struct_size;
	temp_new->is_allocated = 0;
	temp_new->prev = temp_old;
	temp_new->next = temp_old->next;
	
	// Modify the old block
	temp_old->size = size;
	temp_old->next = temp_new;
	temp_old->is_allocated = 1;
	
	return temp_new->space;
}


void *worst_fit_alloc(size_t size)
{
	struct mem_ll* temp_old;
	struct mem_ll* temp_new = malloc(sizeof(struct mem_ll));
	
	if(size%4) { 
		size += 4 - size%4; 
	}
	// Look through the list to try to find a suitable block
	temp_old = ll_find_max_block( ll_worst, size + ll_struct_size );
	
	// If we did not find an adequate block
	if(temp_old == 0) {
		return NULL;
	}
	printf("worst: found element of size %d at %d\n", temp_old->size, (int)temp_old->space); fflush(stdout); 
	
	// Break the found memory block into two blocks: the original found block
	// will now be allocated and of the specified size, and the remaining space will 
	// be unallocated and with the remaining size
	
	// Create the new block after the original block
	temp_new->size = temp_old->size - size - ll_struct_size;
	temp_new->space = temp_old->space + size + ll_struct_size;
	temp_new->is_allocated = 0;
	temp_new->prev = temp_old;
	temp_new->next = temp_old->next;
	
	// Modify the old block
	temp_old->size = size;
	temp_old->next = temp_new;
	temp_old->is_allocated = 1;
	
	return temp_new->space;
}

/* memory de-allocator */
void best_fit_dealloc(void *ptr) 
{
	struct mem_ll* temp;
	
	if(!ptr) {
		return;
	}

	// Find the allocated memory location
	temp = ll_find_ptr(ll_best, ptr);
	
	// We didn't successfully find the memory location in our linked list
	if(!temp) {
		return;
	}
	
	printf("best: deallocating element\n"); fflush(stdout);
	
	// We found the location, now coalesce: 
	// Check if the next ll element is unallocated
	if(temp->next) {
		// If the next element is unallocated, absorb into this one
		if(!temp->next->is_allocated) {
			temp->size += temp->next->size + ll_struct_size;
			temp->next = temp->next->next;
		}
	}
	
	// Recursively move back and try to coalesce previous elements
	while(temp->prev) {
		temp = temp->prev;
		if(!temp->is_allocated) {
			temp->size += temp->next->size + ll_struct_size;
			temp->next = temp->next->next;
		}
	}
	return;
}

void worst_fit_dealloc(void *ptr) 
{
	struct mem_ll* temp;
	
	if(!ptr) {
		return;
	}
	// Find the allocated memory location
	temp = ll_find_ptr(ll_worst, ptr);
	
	// We didn't successfully find the memory location in our linked list
	if(!temp) {
		return;
	}
	printf("worst: deallocating element\n"); fflush(stdout);
	
	// We found the location, now coalesce: 
	// Check if the next ll element is unallocated
	if(temp->next) {
		
		// If the next element is unallocated, absorb into this one
		if(!temp->next->is_allocated) {
			temp->size += temp->next->size + ll_struct_size;
			temp->next = temp->next->next;
		}
	}
	
	// Recursively move back and try to coalesce previous elements
	while(temp->prev) {
		temp = temp->prev;
		if(!temp->is_allocated) {
			temp->size += temp->next->size + ll_struct_size;
			temp->next = temp->next->next;
		}
	}
	return;
}

/* memory algorithm metric utility function(s) */

/* count how many free blocks are less than the input size */ 
int best_fit_count_extfrag(size_t size)
{
	struct mem_ll* current = ll_best;
	int frag_count = 0;
	
	while (current->next) {
		printf("checking next\n"); fflush(stdout);
		if(current->size < size && !current->is_allocated) {
			frag_count++;
		}
		current = current->next;
	}
	
	if(current->size < size && !current->is_allocated) {
		frag_count++;
	}
	
	return frag_count;
}

int worst_fit_count_extfrag(size_t size)
{
	struct mem_ll* current = ll_worst;
	int frag_count = 0;
	
	while (current->next) {
		if(current->size < size && !current->is_allocated) {
			frag_count++;
		}
		current = current->next;
	}
	
	if(current->size < size && !current->is_allocated) {
			frag_count++;
	}
	
	return frag_count;
}
