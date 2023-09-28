#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Hey, Buddy, you choose the wrong door, the parallel club has two args of command line\n");
		return 0;
	}

	int num = atoi(argv[1]);

	int worldSize = 0, worldRank = 0;

	MPI_Init(&argc, &argv);
	MPI_Group worldGroup, myGroup;
	MPI_Comm myCommunicator;

	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);

	const int extraRanks[] = {worldSize - 1};

	MPI_Group_excl(worldGroup, 1, extraRanks, &myGroup);

	MPI_Comm_create(MPI_COMM_WORLD, myGroup, &myCommunicator);

	if (worldRank != worldSize - 1) {
		int rank = 0, size = 0;
		MPI_Comm_rank(myCommunicator, &rank);
		MPI_Comm_size(myCommunicator, &size);

		printf("wolldSize = %d, worldRank = %d, commSize = %d, commRank = %d\n", worldSize, worldRank, size, rank);

		double sum = 0;
		for (int i = num / size * rank + 1, end = (rank == size - 1) ? num + 1 : num / size * (rank + 1) + 1; i < end; ++i) {
			sum += 1.0 / i;
		}
		double result = 0;

		MPI_Reduce(&sum, &result, 1, MPI_DOUBLE, MPI_SUM, 0, myCommunicator);

		if (!rank)
			printf("res = %lf\n", result);
	}

	MPI_Group_free(&myGroup);
	MPI_Group_free(&worldGroup);

	MPI_Finalize();
	return 0;
}
