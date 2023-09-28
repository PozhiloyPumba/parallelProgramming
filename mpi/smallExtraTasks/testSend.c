#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#ifndef N
#define N 65536	 // 65480 is border on my machine for default send
#endif

#ifndef MODE
#define SEND MPI_Send
#else
#if (MODE == S)
#define SEND MPI_Ssend
#elif (MODE == R)
#define SEND MPI_Rsend
#elif (MODE == B)
#define SEND MPI_Bsend
#endif
#endif

int main(int argc, char *argv[]) {
	int worldSize = 0, worldRank = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

	if (worldSize < 2) {
		printf("Hey, Buddy, you choose the wrong door, this prog must have 2 proc at least\n");
		return 0;
	}

	char array[N] = {};
	struct timeval stop, start;

	for (int i = N; i > 0; i >>= 1) {
		if (!worldRank)
			printf("size = %d\n", i);
		switch (worldRank) {
			case 0: {
				MPI_Status st;
				MPI_Barrier(MPI_COMM_WORLD);

				gettimeofday(&start, NULL);
				sleep(2);
				gettimeofday(&stop, NULL);

				MPI_Recv(array, i, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &st);

				printf("\tdelay was %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

				break;
			}
			case 1: {
				MPI_Barrier(MPI_COMM_WORLD);

				gettimeofday(&start, NULL);

				SEND(array, i, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

				gettimeofday(&stop, NULL);

				printf("\tin send %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
				break;
			}
			default:
				MPI_Barrier(MPI_COMM_WORLD);
		}
		fflush(stdout);
	}

	MPI_Finalize();
	return 0;
}
