#include <stdlib.h>

struct mem_ll {
	struct mem_ll* next;	// Pointer to the next element in the linked list
	struct mem_ll* prev;	// Pointer to the previous element in the linked list
	int is_allocated;		// Whether or not this memory location is allocated
	size_t size;			// The size of the memory space in this linked list element
	void* space;			// Pointer to the allocated memory regulated by this element
};

/* 	Looks through the given linked list and finds the smallest unallocated element of at least
	the given size
*/
struct mem_ll* ll_find_min_block ( struct mem_ll* ll, size_t size ) {
	struct mem_ll* current = ll;
	struct mem_ll* min = current;

	while (current->next) {
		current = current->next;
		if(current->size >= size && current->size <= min->size && !current->is_allocated) {
			min = current;
		}
	}
	
	if(min->size >= size) {
		return min;
	}
	
	else return 0;
}

/* 	Looks through the given linked list and finds the largest unallocated element of at least
	the given size
*/
struct mem_ll* ll_find_max_block ( struct mem_ll* ll, size_t size ) {
	struct mem_ll* current = ll;
	struct mem_ll* max = current;

	while (current->next != 0) {
		current = current->next;
		if(current->size >= size && current->size >= max->size && !current->is_allocated) {
			max = current;
		}
	}
	
	if(max->size >= size) {
		return max;
	}
	
	else return 0;
}

/* 	Looks through the given linked list and finds an element containing the 
	space pointed to by the given pointer
*/
struct mem_ll* ll_find_ptr ( struct mem_ll* ll, void* space ) {
	struct mem_ll* current = ll;
	
	while (current->next != 0) {
		if(current->space == space) {
			return current;
		}
		current = current->next;
	}
	
	return 0;
}
