#ifndef _SENSE_REVERSE_OMP

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int NUM_THREADS, NUM_BARRIERS;

void sense_reversing_barrier_OpenMP (int *count, int *sense);
void sense_reversing_barrier_OpenMP_init (int count, int barriers);

#endif