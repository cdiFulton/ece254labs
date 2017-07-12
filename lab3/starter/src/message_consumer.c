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

int main( int argc, char *argv[] ) 
{
	struct for_consumer* cons_params;
	int* receive_buffer;
	unsigned prio = 1;
	double consumed = -1;
	double root;
	
	if( argc < 5 ) {
		return -1;
	}
	
	cons_params->cons_ID = atoi( argv[2] );
	cons_params->mq = (mqd_t)atoi( argv[4] );
	
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
