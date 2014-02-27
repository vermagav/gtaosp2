#include "sense_reversing_barrier.h"

static void sense_reversing_barrier_OpenMP (int *count, int *sense){
	
	// Maintain local sense
	int local_sense = *sense;

	// Perform atomic decrement
	#pragma omp critical{
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

static void sense_reversing_barrier_OpenMP (int count, int barriers) {
	NUM_THREADS = count;
	NUM_BARRIERS = barriers;
}
