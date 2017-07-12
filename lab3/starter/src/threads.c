#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <spawn.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <math.h>
#include "helpers.h"


int num_producers_done = 0;
int production_done = 0;

sem_t* BUFFER_MUTEX;

struct for_producer {
	int num_to_prod;	// How many values are to be produced
	int prod_ID;		// ID of this producer
	int num_prod;		// How many producers there are
	int* buffer;		// Pointer to the buffer
};

struct for_consumer {
	int cons_ID;		// ID of this consumer
	int* buffer;		// Pointer to the buffer
};

// gcc -g -lpthread -lm -lrt -o produce threads.c helpers.c

/* 	A producer thread. Produces numbers and places them in a buffer.
	Parameters: 
		num_to_produce is the number of integers the producer will produce
		buffer is the buffer that this producer will place its numbers in
		pid is the ID of this producer
		num_producers is the total number of producers
	Return:
		void 
*/
void* producer( void* data_in ) 
{
	struct for_producer* prod_params;
	// The number that will be produced
	int produced;
	
	prod_params = (struct for_producer*)data_in;
	
	produced = prod_params->prod_ID;
	
	
	while (produced < prod_params->num_to_prod) {
		//printf("num: %d, produced: %d \n", prod_params->num_to_prod, produced);
		sem_wait( BUFFER_MUTEX );
		//printf("pushed value: %d \n", produced);
		if( !(push( prod_params->buffer, produced ) == -1) ) {
			produced += prod_params->num_prod;
		}
		sem_post( BUFFER_MUTEX );
	}
	
	// This producer is done all of its production. 
	sem_wait( BUFFER_MUTEX );
	num_producers_done++;
	// If all producers are done, then raise a flag for the consumers
	if( num_producers_done == prod_params->num_prod ) {
		production_done = 1;
	}
	sem_post( BUFFER_MUTEX );
	
	pthread_exit(0);
}

/* 	A consumer thread. Consumes numbers from a buffer and prints their square root.
	Parameters: 
		num_to_consume is the number of integers the producer will consume
		buffer is the buffer that this producer will retrieve its numbers from
		cid is the ID of this consumer
	Return:
		void 
*/
void* consumer( void* data_in ) 
{
	struct for_consumer* cons_params;
	double consumed = -1;
	double root;
	
	cons_params = (struct for_consumer*)data_in;
	
	
	while( !(production_done && consumed == -1) ) {
		//printf("consumed: %d, cid: %d \n", consumed, cons_params->cons_ID);
		sem_wait( BUFFER_MUTEX );
		consumed = (double) pop( cons_params->buffer );
		sem_post( BUFFER_MUTEX );
		if( consumed != -1 ) {
			root = sqrt( consumed );
			if( round( root ) == root ) {
				printf("%d %d %d \n", cons_params->cons_ID, (int)consumed, (int)root);
			}
		}
	}
	
	pthread_exit(0);
}

int main(int argc, char *argv[])
{
	int n, b, p, c, i, j, error_code;
	double start_time, end_time;
	struct timeval tv;
	int* buffer;
	struct for_producer* data_prod;
	struct for_consumer* data_cons;
	pthread_t* prod_threads;
	pthread_t* cons_threads;
	
	
	// Check  the command line arguments. If there aren't enough, error
	if( argc < 5 ) {
		return -1;
	}
	
	n = atoi( argv[1] );
	b = atoi( argv[2] );
	p = atoi( argv[3] );
	c = atoi( argv[4] );
	
	// Initialize buffer, get start time before creating producers and consumers
	buffer = init_buffer( b );
	if (buffer == 0) {
		return -1;
	}
	
	
	BUFFER_MUTEX = malloc( sizeof(sem_t) );
	if ((error_code = sem_init(BUFFER_MUTEX, 0, 1))) {
		printf("Error initializing semaphore: error %d\n", error_code);
		return -1;
	}
	
	gettimeofday(&tv, NULL);
	start_time = tv.tv_sec + tv.tv_usec/1000000.0;
	
	
	// Initialize producers
	prod_threads = (pthread_t*)malloc( sizeof(pthread_t)*p );
	data_prod = (struct for_producer*)malloc( sizeof(struct for_producer)*p );
	for (i = 0; i < p; i++) {
		data_prod[i].num_to_prod = n;
		data_prod[i].prod_ID = i;
		data_prod[i].num_prod = p;
		data_prod[i].buffer = buffer;
		
		pthread_create( &prod_threads[i], NULL, producer, (void*)&data_prod[i] );
	}
	
	// Initialize consumers
	cons_threads = (pthread_t*)malloc( sizeof(pthread_t)*c );
	data_cons = (struct for_consumer*)malloc( sizeof(struct for_consumer)*c );
	for (j = 0; j < c; j++) {
		data_cons[j].cons_ID = j;
		data_cons[j].buffer = buffer;
		
		pthread_create( &cons_threads[j], NULL, consumer, (void*)&data_cons[j]  );
	}
	
	// Wait for all consumers to finish. 
	// Consumers wait for producers so we don't need to wait for producers
	for (j = 0; j < c; j++) {
		pthread_join( cons_threads[j], NULL );
	}
	
	gettimeofday(&tv, NULL);
	end_time = tv.tv_sec + tv.tv_usec/1000000.0;
	
	printf("System execution time: %.6lf \n", end_time - start_time);
	
	// Free allocated memory
	free(prod_threads);
	free(data_prod);
	free(cons_threads);
	free(data_cons);
	free(buffer);
}



