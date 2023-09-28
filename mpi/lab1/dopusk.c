#include <mpi.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv) {
	// Initialize the MPI environment
	MPI_Init(&argc, &argv);

	// Get the number of processes
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// Get the rank of the process
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int N = 10000000;
	int a = 10;
	if (rank == 0) {
		clock_t start = clock();
		for (int i = 0; i < N; ++i)
			MPI_Send(&a, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		clock_t end = clock();
		float milliseconds = (float)(end - start) / CLOCKS_PER_SEC * 1000;
		printf("time = %lfms\n", milliseconds / N);
	}
	if (rank == 1) {
		MPI_Status st;
		for (int i = 0; i < N; ++i)
			MPI_Recv(&a, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &st);
	}
	// Finalize the MPI environment.
	MPI_Finalize();
}
