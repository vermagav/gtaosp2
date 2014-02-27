#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum { false, true } bool;

void message(int left, int right) {
	printf("> %d received message from %d\n", left, right); // TODO(narang): replace with actual MPI message
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
		printf("Error: Invaalid number of processes.\n");
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
	for(int i = 0; i < n; i++) {
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
		for(int i = 0; i < n; i += level_jump) {
			int left = i;
			int right = i + right_pair_distance;
			if(tree[left].active == true && tree[right].active == true) {
				message(tree[left].pid, tree[right].pid);
			}
		}
		right_pair_distance += right_pair_distance;
		level_jump += level_jump;
		printf("\n"); // TODO(narang): remove this print message once we're done debugging
	}
}


void barrier() {
	// TODO
}

int main() {
	int n = 7;
	barrier_init(n);

	return 0;
}