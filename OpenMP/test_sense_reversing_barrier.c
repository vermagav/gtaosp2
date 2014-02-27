#include <omp.h>
#include "sense_reversing_barrier.h"

int main (int argc, char **argv)
{
	int threads,num_barrier;
	if( argc == 3 ){
	  threads = atoi( argv[1] );
	  num_barrier = atoi( argv[2] );
	  sense_reversing_barrier_OpenMP_init(threads, num_barrier);
	}

	else{ 
	  printf("Syntax:\n./sense_reversing_barrier num_threads num_barriers\n");
	  exit(-1);
	}

	// Serial code
	printf("This is the serial section\n");
	omp_set_num_threads( threads );
	int sense = 1, count = threads;
	double time1, time2;

	#pragma omp parallel shared(sense, count) {
		
		// Parallel section
		int num_threads = omp_get_num_threads();
		int thread_num = omp_get_thread_num();
		long i,j;
		double total_time;

		for(j=0; j<NUM_BARRIERS; j++){
			printf("Hello World from thread %d of %d.\n", thread_num, num_threads);

			time1 = omp_get_wtime();
			
			// Barrier
			sense_reversing_barrier_OpenMP(&count, &sense);
			
			time2 = omp_get_wtime();

			total_time += time2 - time1;
			
			printf("Bye-Bye World from thread %d of %d.\n", thread_num, num_threads);
		}

		printf("Time spent in barrier(s) by thread %d is %f\n", thread_num, total_time);
	}

	printf("Back in the serial section again\n");
	return 0;
}