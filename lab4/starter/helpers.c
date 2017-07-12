#include <stdlib.h>

struct mem_ll_head {
	size_t tot_size;			// The total size of all available spaces in this linked list
	struct mem_ll* first;	// Pointer to the first element in the linked list
}

struct mem_ll {
	size_t size;			// The size of the memory space in this linked list element
	struct mem_ll* next;	// Pointer to the next element in the linked list
	struct mem_ll* prev;	// Pointer to the previous element in the linked list
	void* space;			// Pointer to the allocated memory regulated by this element
}

/* 	Looks through the given linked list and finds the smallest element of at least
	the given size
*/
struct mem_ll* ll_find_min_block ( mem_ll* ll, size_t size ) {
	struct mem_ll* current = ll;
	struct mem_ll* min;
	min->size = 0;
	while (current->next != 0) {
		if(current->size >= size && current->size <= min->size) {
			min = current;
		}
		current = current->next;
	}
}

/* 	Looks through the given linked list and finds the largest element of at least
	the given size
*/
struct mem_ll* ll_find_max_block ( mem_ll* ll, size_t size ) {
	struct mem_ll* current = ll;
	struct mem_ll* min;
	min->size = 0;
	while (current->next != 0) {
		if(current->size >= size && current->size >= min->size) {
			min = current;
		}
		current = current->next;
	}
	
	if(min->size) {
		return min;
	}
	else return 0;
}

/* 	Looks through the given linked list and finds an element containing the 
	space pointed to by the given pointer
*/
struct mem_ll* ll_find_ptr ( mem_ll* ll, void* space ) {
	struct mem_ll* current = ll;
	
	while (current->next != 0) {
		if(current->space == space) {
			return current;
		}
		current = current->next;
	}
	
	return 0;
}
