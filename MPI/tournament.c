#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define MAXMESSAGES 50

#define DUMMY_SIZE 1
#define DUMMY_TAG 1

typedef enum { false, true } bool;

int messages[MAXMESSAGES][2];
int num_messages = 0;

void message(int left, int right) {
	messages[num_messages][0]=left;
	messages[num_messages][1]=right;
	num_messages++;
}

// A node used in our pseudo-binary tree
typedef struct node {
	// Process ID
	int pid;
	
	// Flag that indicates if the node is currently active in our tree
	bool active;
} node_t;

void barrier_init(int num_procs) {

	if(num_procs < 1) {
		printf("Error: Invalid number of processes.\n");
		exit(0);
	}

	// Determine nearest power of 2 >= num_procs
	int power = 1;
	while(pow(2, power) <= num_procs) {
		power++;
	}

	// Construct pseudo-tree, array holding lowest depth
	int n = pow(2, power);
	struct 	node tree[n];
	int i;
	for(i = 0; i < n; i++) {
		tree[i].pid = i;
		if(i < num_procs) {
			tree[i].active = true;
		} else {
			tree[i].active = false;
		}
	}

	// The following code simulates upward traversal
	// of a binary tree using a simple 2D array.
	int level_jump = 2;
	int right_pair_distance = 1;
	while (level_jump <= n) {
		for(i = 0; i < n; i += level_jump) {
			int left = i;
			int right = i + right_pair_distance;
			if(tree[left].active == true && tree[right].active == true) {
				message(tree[left].pid, tree[right].pid);
			}
		}
		right_pair_distance += right_pair_distance;
		level_jump += level_jump;
	}
}


void barrier(int rank) {

	int iter, message = 1;
	bool done = false;
	// Loop through the messages array to send/receive messages
	for (iter = 0; iter < num_messages; iter++) {
		if (messages[iter][0] == rank) {
			MPI_Recv(&message, DUMMY_SIZE, MPI_INT, messages[iter][1], DUMMY_TAG , MPI_COMM_WORLD, NULL);
		}
		else if (messages[iter][1] == rank) {
			MPI_Send(&message, DUMMY_SIZE, MPI_INT, messages[iter][0], DUMMY_TAG , MPI_COMM_WORLD);
			break;
		}
	}

	// Once process 0 wins the tourney, it tells everyone that it won.
	if (rank == 0) {
		done = true;
		if (num_messages != 0) {
			MPI_Bcast(&done, DUMMY_SIZE, MPI_INT, rank, MPI_COMM_WORLD);
		}
	}

	// Spin on local variable done
	while(!done);
}

int main(int argc, char **argv) {

	int my_id, num_processes;
	MPI_Init(&argc, &argv);

  	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
  	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

	barrier_init(num_processes);

  	fprintf(stderr, "Before barrier 1 : Process %d of %d\n", my_id+1, num_processes);
	fflush(stderr);

	// Barrier
	barrier(my_id);

	fflush(stderr);
  	fprintf(stderr, "Before Barrier 2 : Process %d of %d\n", my_id+1, num_processes);
	fflush(stderr);
	
	// Barrier
	barrier(my_id);
	
	fflush(stderr);
  	fprintf(stderr, "After barrier 2 : Process %d of %d\n", my_id+1, num_processes);
	fflush(stderr);


  	MPI_Finalize();
  	return 0;
}