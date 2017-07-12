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

int main ( int argc, char *argv[] ) 
{
	printf("checkprod"); fflush(stdout);
	struct for_producer* prod_params;
	// The number that will be produced
	int produced;
	unsigned prio = 1;
	
	if( argc < 5 ) {
		return -1;
	}
	printf("%s %s %s %s", argv[1], argv[2], argv[3], argv[4]);
	prod_params->num_to_prod = atoi( argv[1] );
	prod_params->prod_ID = atoi( argv[2] );
	prod_params->num_prod = atoi( argv[3] );
	prod_params->mq = (mqd_t)atoi( argv[4] );
	
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