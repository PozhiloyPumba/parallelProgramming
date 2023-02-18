#include <mpi.h>
#include <stdio.h>
#include "countEuler.h"

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Hey, Buddy, you choose the wrong door, the parallel club has two args of command line\n");
		return 0;
	}

	MPI_Init(&argc, &argv);
	
	mpf_t ans; mpf_init(ans);

	countE(argc, argv, ans);// TODO: count Euler number to border

	MPI_Finalize();

	// TODO: print answer
}
