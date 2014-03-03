#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int NUM_THREADS, NUM_BARRIERS;

void sense_reversing_barrier_OpenMP (int *count, int *sense){
	
	// Maintain local sense
	int local_sense = *sense;

	// Perform atomic decrement
	#pragma omp critical 
	{
		*count = (*count) - 1;
	}

	// If last thread, then flip local sense
	if( *count == 0 ){
		*count = NUM_THREADS;
		// Flip the sense variable on the shared memory
		*sense = !local_sense;
	}
	else {
		// Spin on sense reversal
		while( *sense == local_sense );
	}
}

void sense_reversing_barrier_OpenMP_init (int count, int barriers) {
	NUM_THREADS = count;
	NUM_BARRIERS = barriers;
}

int main (int argc, char **argv)
{
	int threads, num_barrier;
	if( argc == 3 ) {
	  threads = atoi( argv[1] );
	  num_barrier = atoi( argv[2] );
	  sense_reversing_barrier_OpenMP_init(threads, num_barrier);
	}

	else{ 
	  printf("Syntax:\n./test_sense_reversing_barrier num_threads num_barriers\n");
	  exit(-1);
	}

	// Serial code
	printf("Welcome to the serial code section...\n");
	omp_set_num_threads( threads );
	int sense = 1, count = threads;

	#pragma omp parallel default(none) shared(NUM_BARRIERS, sense, count) 
	{
		
		// Parallel section
		int num_threads = omp_get_num_threads();
		int thread_num = omp_get_thread_num();
		long j;
		double total_time = 0;
		double time1, time2;

		for( j=0; j<NUM_BARRIERS; j++ ) {
			printf("START POINT, thread %d of %d\n", thread_num, num_threads);

			time1 = omp_get_wtime();
			
			// Barrier
			sense_reversing_barrier_OpenMP(&count, &sense);
			
			time2 = omp_get_wtime();

			total_time += time2 - time1;
			
			printf("END POINT, thread %d of %d\n", thread_num, num_threads);
		}

		printf("Time spent by thread %d: %f\n", thread_num, total_time);
	}

	printf("End of execution and back to serial code section...\n");
	return 0;
}
