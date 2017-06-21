#include <stdlib.h>

/* 	A producer thread. Produces numbers and places them in a buffer.
	Parameters: 
		num_to_produce is the number of integers the producer will produce
		buffer is the buffer that this producer will place its numbers in
		pid is the ID of this producer
		num_producers is the total number of producers
	Output:
		void 
*/
void producer( int num_to_produce, int* buffer, int pid, int num_producers ) 
{
	
}

/* 	A consumer thread. Consumes numbers from a buffer and prints their square root.
	Parameters: 
		num_to_consume is the number of integers the producer will consume
		buffer is the buffer that this producer will retrieve its numbers from
		cid is the ID of this consumer
	Output:
		void 
*/
void consumer( int num_to_consume, int* buffer, int cid ) 
{
	
}

/* 	Initializes the buffer; simple function to create an int array to 
	act as the buffer.
	Parameters: 
		buffer_size is the number of integers the buffer should hold
	Output:
		A pointer to the buffer. Returns nullptr on error. 
*/
int* init_buffer( int buffer_size ) 
{
	int* ptr;
	
	// It only makes sense to allocate a buffer of positive size, so 
	// return error code if buffer_size is not positive
	if( buffer_size <= 0 ) {
		return 0;
	}
	
	// Each buffer array contains the number of ints specified plus one:
	// the 0th position in the buffer is used as an index for the buffer
	// to keep track of which slot we're at
	ptr = (int*) malloc( sizeof(int)*(buffer_size + 1) );
	if ( 0 == ptr ){
		return 0;
	}
	// Initialize the index of the buffer to 1, the first available position
	ptr[0] = 1;
	
	return ptr;
}

/* 	Dequeues a number from the given buffer. Moves backwards and then 
	dequeues the value at that location. 
	Parameters:
		buffer is a pointer to the buffer that will be dequeued from
	Output:
		The number dequeued from the buffer. Returns -1 on error. 
*/
int dequeue( int* buffer ) 
{
	// We're at the first non-index location in the buffer, 
	// so there's nothing to dequeue
	if ( 1 == buffer[0] ) {
		return -1;
	}
	// Decrement the index, then take the value at the indexed location
	else {
		buffer[0]--;
		return buffer[buffer[0]];
	}
}

/* 	Enqueues a number on the given buffer. Enqueues the value at the current 
	location then moves forwards.
	Parameters:
		buffer is a pointer to the buffer that will be enqueued on
		value is the value to be enqueued on the buffer
	Output:
		Returns -1 on error, 0 otherwise. 
*/
int enqueue( int* buffer, int value ) 
{
	int buffer_size = sizeof(buffer)/sizeof(buffer[0]);
	// We're at the end of the buffer, so there's no room to enqueue
	if ( buffer_size == buffer[0] ) {
		return -1;
	}
	// Writes the value to the buffer at the current index, then increments the index
	else {
		buffer[buffer[0]] = value;
		buffer[0]++;
		return 0;
	}
}