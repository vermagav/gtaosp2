#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <omp.h>
#define MAXTHREADS 100
#define MAXROUNDS 10
#define DUMMY_SIZE 1
#define DUMMY_TAG 1
#define NUM_THREADS_PER_PROC 2
#define NUM_BARRIER 1

int num_threads;
int rounds;
int messages[MAXTHREADS][MAXROUNDS];


void sense_reversing_barrier_MPI(int *rank, int *numprocs, int *sense){
	
	int message = 1, i;
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
}


void dissemination_barrier_init(int thread_count){
	
	int i,j;

	num_threads = thread_count;
	rounds = ceil(log(thread_count)/log(2));

	// Initialize all messages to 0 in the beginning
	for (i = 0; i< rounds; i++) {
		for (j = 0; j< num_threads; j++) {
			messages[i][j] = 0; 
		}
	}
}

void dissemination_barrier(){
	
	int i, temp_id;
	int my_id = omp_get_thread_num();

	for (i = 0; i < rounds; i++){
		
		// calculate whom to send the next message to
		temp_id = (my_id + (int)pow(2,i))%num_threads;

		// spin on sent message
		while(messages[i][temp_id] == 1);
		messages[i][temp_id] = 1;

		// spin on receiving the message
		while(messages[i][my_id] == 0);
		messages[i][my_id] = 0;
	}

}

int main(int argc, char **argv) {
	
	//MPI PART - INITIALIZATIONS
	int i, rank, numprocs;
	double time3, time4, timemp=0, timempi=0;
	int sense =0;
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//MPI INITIALIZATIONS END

  	fprintf(stderr, "Before MPI barrier  : Process %d of %d\n", rank+1, numprocs);
	time3 = MPI_Wtime();
	sense_reversing_barrier_MPI(&rank, &numprocs, &sense);
	time4 = MPI_Wtime();
	timempi = time4 - time3;
	printf("MPI time %d\n", timempi);
	fprintf(stderr, "After MPI Barrier  : Process %d of %d\n", rank+1, numprocs);
	
	//OpenMP PART
	double time1, time2,total_time = 0;
	omp_set_num_threads(NUM_THREADS_PER_PROC);
	int num_barriers = NUM_BARRIER;

	#pragma omp parallel shared (total_time, num_barriers) private(time1,time2)
	{

		int thread_num = omp_get_thread_num();
		int numthreads = omp_get_num_threads();
		int j;
		for(j=0; j<num_barriers; j++){

			time1 = omp_get_wtime();
			printf("Before Barrier %d for thread %d\n", j+1 , thread_num);
			// Barrier
			dissemination_barrier();
			
			time2 = omp_get_wtime();

			printf("After Barrier %d for thread %d\n", j+1 , thread_num);
			total_time += time2 - time1;
			
		}
	}
	printf("The total time is %d\n", total_time);

}

