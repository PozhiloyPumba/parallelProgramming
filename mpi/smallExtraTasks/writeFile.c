#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main (int argc, char *argv[]) {
	int worldSize = 0, worldRank = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

	MPI_File f;
	MPI_Status st;

	MPI_File_open(MPI_COMM_WORLD, "sync.txt", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &f);

	for (int i = worldSize - 1; i > worldRank; --i)
		MPI_Barrier(MPI_COMM_WORLD);

	char str[10];
	int len = sprintf(str, "%d", worldRank);
	str[len] = '\n';
	int seek = 0;

	if (worldRank != worldSize - 1)
		MPI_Recv(&seek, 1, MPI_INT, worldRank + 1, 0, MPI_COMM_WORLD, &st);

	MPI_File_write_at(f, seek, &str, len+1, MPI_CHAR, &st);
	
	seek += len+1;
	if (worldRank)
		MPI_Send(&seek, 1, MPI_INT, worldRank - 1, 0, MPI_COMM_WORLD);

	for (int i = worldRank; i >= 0; --i)
		MPI_Barrier(MPI_COMM_WORLD);

	MPI_File_close(&f);
	MPI_Finalize();
	return 0;
}
