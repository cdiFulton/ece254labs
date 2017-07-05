#include <stddef.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/wait.h>
#include <spawn.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>
#include "helpers.h"

int num_producers_done = 0;
int production_done = 0;

struct for_producer {
	int num_to_prod;	// How many values are to be produced
	int prod_ID;		// ID of this producer
	int num_prod;		// How many producers there are
	mqd_t mq;			// Pointer to the mqueue
};

struct for_consumer {
	int cons_ID;		// ID of this consumer
	mqd_t mq;			// Pointer to the mqueue
};

void* producer( char* data_in ) 
{
	struct for_producer* prod_params;
	// The number that will be produced
	int produced;
	unsigned prio = 1;
	
	prod_params = (struct for_producer*)data_in;
	produced = prod_params->prod_ID;
	
	printf("Initialized producer %d\n", prod_params->prod_ID);
	
	while (produced < prod_params->num_to_prod) {
		if (0 == mq_send(prod_params->mq, (char*)&produced, sizeof(int), prio)) {
			produced += prod_params->num_prod;
		}
	}
	
	// This producer is done all of its production. 
	num_producers_done++;
	// If all producers are done, then raise a flag for the consumers
	if( num_producers_done == prod_params->num_prod ) {
		production_done = 1;
	}
	
	return 0;
}

void* consumer( char* data_in ) 
{
	struct for_consumer* cons_params;
	int* receive_buffer;
	unsigned prio = 1;
	double consumed = -1;
	double root;
	
	cons_params = (struct for_consumer*)data_in;

	printf("Initialized consumer %d\n", cons_params->cons_ID);
	
	while( !(production_done && consumed == -1) ) {
		if (-1 != mq_receive( cons_params->mq, (char*)receive_buffer, sizeof(int), &prio )) {
			consumed = *(int*)receive_buffer;
		}
		if( consumed != -1 ) {
			root = sqrt( consumed );
			if( round( root ) == root ) {
				printf("%d %d %d\n", cons_params->cons_ID, consumed, root);
			}
		}
	}
	
	return 0;
}

// todo: makefile with gcc -lpthread -lm -lrt -o produce threads.c helpers.c
///////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	int n, b, p, c, i, j;
	float start_time, end_time;
	struct timeval tv;
	struct for_producer* data_prod;
	struct for_consumer* data_cons;
	mqd_t mq;
	int* prod_processes;
	int* cons_processes;
	int wait_status;
	char* prod_file_name = "message_producer";
	char* cons_file_name = "message_consumer";
	
	// Get command line arguments:
	// n = number to produce
	// b = buffer size
	// p = number of producers
	// c = number of consumers
	if( argc < 5 ) {
		return -1;
	}
	
	n = atoi( argv[1] );
	b = atoi( argv[2] );
	p = atoi( argv[3] );
	c = atoi( argv[4] );
		
	fflush(stdout);
	// Initialize mailbox, get start time before creating producers and consumers
	mq = init_mqueue( b );
	
	gettimeofday(&tv, NULL);
	start_time = tv.tv_sec + tv.tv_usec/1000000.0;
	
	fflush(stdout);
	// Initialize producers
	prod_processes = (int*)malloc( sizeof(int)*p );
	data_prod = (struct for_producer*)malloc( sizeof(struct for_producer)*c );
	for (i = 0; i < p; i++) {
		data_prod[i].num_to_prod = n;
		data_prod[i].prod_ID = i;
		data_prod[i].num_prod = p;
		data_prod[i].mq = mq;
		
		prod_processes[i] = fork();
		if (prod_processes[i] < 0) {
			fprintf(stderr, "Fork Failed");
			return -1;
		}
		else if (prod_processes[i] == 0) {
			printf("Trying to initialize producer %d\n", data_prod[i].prod_ID);
			execvp(prod_file_name, data_prod[i]);
		}
	}
	
	// Initialize consumers
	// initialize an array of process IDs for the consumers because we need to wait
	// for all of the consumers to finish
	cons_processes = (int*)malloc( sizeof(int)*p );
	data_cons = (struct for_consumer*)malloc( sizeof(struct for_consumer)*c );
	for (j = 0; j < c; j++) {
		data_cons[j].cons_ID = j;
		data_cons[j].mq = mq;
		
		cons_processes[j] = fork();
		if (cons_processes[j] < 0) {
			fprintf(stderr, "Fork Failed");
			return -1;
		}
		else if (cons_processes[j] == 0) {
			printf("Trying to initialize consumer %d\n", data_cons[i].cons_ID);
			execvp(cons_file_name, data_cons[i]);
		}
	}
	
	// Wait for all consumers to finish. 
	// Consumers wait for producers so we don't need to wait for producers
	for (j = 0; j < c; j++) {
		waitpid(cons_processes[j], &wait_status, 0);
	}
	
	gettimeofday(&tv, NULL);
	end_time = tv.tv_sec + tv.tv_usec/1000000.0;
	
	printf("System execution time: %.6lf \n", end_time - start_time);
	
	free(prod_processes);
	free(data_prod);
	free(cons_processes);
	free(data_cons);
	mq_close(mq);
}
	