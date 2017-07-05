#include <mqueue.h>

int* init_buffer( int buffer_size );
int pop( int* buffer );
int push( int* buffer, int value );

mqd_t init_mqueue( int q_size );