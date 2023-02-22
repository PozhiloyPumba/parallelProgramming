#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "countEuler.h"

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Hey, Buddy, you choose the wrong door, the parallel club has two args of command line\n");
		return 0;
	}

	MPI_Init(&argc, &argv);
	
	mpf_t ans; mpf_init(ans);

	countE(argc, argv, ans);
	
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (!rank)
		gmp_printf("e = %.*Ff\n", atoll(argv[1]), ans);

	mpf_clear(ans);
	MPI_Finalize();
}
