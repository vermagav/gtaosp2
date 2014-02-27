#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define MAXTHREADS 100
#define MAXROUNDS 10

int num_threads;
int rounds;
int messages[MAXTHREADS][MAXROUNDS];

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

int main (int argc, char **argv)
{
	int threads, num_barrier;
	if( argc == 3 ){
	  threads = atoi( argv[1] );
	  num_barrier = atoi( argv[2] );
	  dissemination_barrier_init(threads);
	}

	else{ 
	  printf("Syntax:\n./dissemination num_threads num_barriers\n");
	  exit(-1);
	}

	// Serial code
	printf("Serial section\n");
	omp_set_num_threads( threads );
	//int count = threads;

	#pragma omp parallel default(none) shared(num_barrier) 
	{
		
		// Parallel section
		int num_threads = omp_get_num_threads();
		int thread_num = omp_get_thread_num();
		int j;
		double total_time;
		double time1, time2;

		for(j=0; j<num_barrier; j++){
			printf("Hello World from thread %d of %d.\n", thread_num, num_threads);

			time1 = omp_get_wtime();
			
			// Barrier
			dissemination_barrier();
			
			time2 = omp_get_wtime();

			total_time += time2 - time1;
			
			printf("Bye-Bye World from thread %d of %d.\n", thread_num, num_threads);
		}

		printf("Time spent in barrier(s) by thread %d is %f\n", thread_num, total_time);
	}

	printf("Back in the serial section again\n");
	return 0;
}
