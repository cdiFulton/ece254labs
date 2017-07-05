#include <stdlib.h>
#include <mqueue.h>
#include <stdio.h>

#define INDEX buffer[0] 

/* 	Initializes the mqueue
	Parameters: 
		q_size is the maximum size of the mqueue
	Output:
		A pointer to the mqueue. Returns nullptr on error. 
*/
mqd_t init_mqueue( int q_size )
{
	fflush(stdout);
	
	mqd_t qdes;
	char  *qname = NULL;
	mode_t mode = S_IRUSR | S_IWUSR;
	struct mq_attr attr;

	qname = "/stuff";

	attr.mq_maxmsg  = q_size;
	attr.mq_msgsize = sizeof(int);
	attr.mq_flags   = 0;	/* a blocking queue  */

	fflush(stdout);
	qdes = mq_open(qname, O_RDWR | O_CREAT, mode, &attr);
	if (qdes == -1 ) {
		perror("mq_open()");
		exit(1);
	}
	
	return qdes;
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

/* 	pops a number from the given buffer. Moves backwards and then 
	pops the value at that location. 
	Parameters:
		buffer is a pointer to the buffer that will be popd from
	Output:
		The number popd from the buffer. Returns -1 on error. 
*/
int pop( int* buffer ) 
{
	// We're at the first non-index location in the buffer, 
	// so there's nothing to pop
	if ( 1 == INDEX ) {
		return -1;
	}
	// Decrement the index, then take the value at the indexed location
	else {
		INDEX--;
		//printf("succesful pop: buffer[%d] returned %d \n", INDEX+1, buffer[INDEX]);
		return buffer[INDEX];
	}
}

/* 	pushs a number on the given buffer. pushs the value at the current 
	location then moves forwards.
	Parameters:
		buffer is a pointer to the buffer that will be pushd on
		value is the value to be pushd on the buffer
	Output:
		Returns -1 on error, 0 otherwise. 
*/
int push( int* buffer, int value ) 
{
	int buffer_size = sizeof(buffer)/sizeof(INDEX);
	// We're at the end of the buffer, so there's no room to push
	if ( buffer_size == INDEX ) {
		//printf("failed to succesfully push: %d \n", value);
		return -1;
	}
	// Writes the value to the buffer at the current index, then increments the index
	else {
		buffer[INDEX] = value;
		INDEX++;
		//printf("succesfully pushed: %d, buffer[%d] is now %d \n", value, INDEX, buffer[INDEX-1]);
		return 0;
	}
}