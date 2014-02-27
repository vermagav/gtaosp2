#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define DUMMY_SIZE 1
#define DUMMY_TAG 1

void sense_reversing_barrier_MPI(int *rank, int *numprocs, int *sense){
	int message = 1, i;
	for (i=0;i<20000;i++);
	
	int root = *(numprocs)-1;
	int local_sense = *sense;
	
	// If not the last process, then send a message to the last process and spin on sense reversal
	if( *rank != *(numprocs)-1 ){
		MPI_Send(&message, DUMMY_SIZE, MPI_INT, root, DUMMY_TAG , MPI_COMM_WORLD);
	}
	else { // If last process, then blocking-receive messages from all other processes

		for(i=0 ; i<root; i++) {
			MPI_Recv(&message, DUMMY_SIZE, MPI_INT, i, DUMMY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		// Flip sense and broadcast it
		*sense = !*sense;
		
	}
	MPI_Bcast(sense, DUMMY_SIZE, MPI_INT, root, MPI_COMM_WORLD);

	// Spin on sense reversal for all processes
	while (local_sense == *sense);
	for (i=0;i<20000;i++);
}

int main(int argc, char **argv)
{
	int my_id, num_processes;
	int sense = 1;
	MPI_Init(&argc, &argv);

  	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
  	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

  	fprintf(stderr, "Before barrier 1 : Process %d of %d\n", my_id+1, num_processes);
	//fflush(stderr);
	sense_reversing_barrier_MPI(&my_id, &num_processes, &sense);
	//fflush(stderr);
  	fprintf(stderr, "Before Barrier 2 : Process %d of %d\n", my_id+1, num_processes);
	//fflush(stderr);
	sense_reversing_barrier_MPI(&my_id, &num_processes, &sense);
	//fflush(stderr);
  	fprintf(stderr, "After barrier 2 : Process %d of %d\n", my_id+1, num_processes);
	//fflush(stderr);

  	MPI_Finalize();
  	return 0;
}