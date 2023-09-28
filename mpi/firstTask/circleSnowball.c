#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int commSize = 0, commRank = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &commSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &commRank);

	if (!commRank) {
		int beg = 0;

		printf("procRank is %d, snowball = %d\n", commRank, beg);

		MPI_Send(&beg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(&beg, 1, MPI_INT, commSize - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Barrier(MPI_COMM_WORLD);

		printf("end of snowball game %d\n", beg);
	} else {
		int snowball;
		MPI_Recv(&snowball, 1, MPI_INT, commRank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		printf("procRank is %d, snowball = %d\n", commRank, ++snowball);

		MPI_Send(&snowball, 1, MPI_INT, (commRank + 1) % commSize, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return 0;
}
