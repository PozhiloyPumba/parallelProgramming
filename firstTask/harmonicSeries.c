#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main (int argc, char *argv[]) {
	if (argc != 2) {
		printf("Hey, Buddy, you choose the wrong door, the parallel club has two args of command line\n");
		return 0;
	}
	
	int num = atoi(argv[1]);
	
	int commSize = 0, commRank = 0;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &commSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &commRank);
	
	double sum = 0;
	for (int i = num / commSize * commRank + 1, end = (commRank == commSize - 1) ? num + 1 : num / commSize * (commRank + 1) + 1; i < end; ++i) {
		//printf("proc %d cycle: %d\n", commRank, i);
		sum += 1.0 / i;
	}
	//printf ("proc %d: %f\n", commRank, sum);
	
	if (commRank) {
        MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    } else {
		double tmp = 0;
		for (int i = 1; i < commSize; ++i) {
			MPI_Recv(&tmp, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			sum += tmp;
		}
		printf ("sum = %lg\n", sum);
	}

	MPI_Finalize();

	return 0;
}
