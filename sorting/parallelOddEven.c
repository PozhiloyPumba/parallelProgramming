#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "oddEvenBase.h"

void split(int *a, const size_t n, int *b, const size_t m) {
    int *temp = (int *)malloc((n + m) * sizeof(int));
    size_t i = 0, j = 0;
    while (i + j < n + m) {
        if (b[j] > a[i]) {
            temp[i + j] = a[i];
			++i;
		}
        else {
            temp[i + j] = b[j];
			++j;
		}
        if ((i == n) || (j == m)) {
			memcpy(temp + i + j, b + j, (m - j) * sizeof(int));
			memcpy(temp + i + j, a + i, (n - i) * sizeof(int));
			break;
		}
    }

    memcpy(a, temp, n * sizeof(int));
    memcpy(b, temp + n, m * sizeof(int));
    free(temp);
}

void parallelOddEven(int *arr, int N) {
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status st;

	int *displs = (int *)malloc(size * sizeof(int));
	int *rcounts = (int *)malloc(size * sizeof(int));
	for (int i = 0; i < size; ++i) {
		int locBegin = N / size * i;
		int locEnd = (i == size - 1) ? N : N / size * (i + 1);
		rcounts[i] = locEnd - locBegin;
		displs[i] = (i == 0) ? 0 : displs[i - 1] + rcounts[i - 1];
	}
	int *localArr = (int *)calloc(rcounts[rank], sizeof(int));
	memcpy(localArr, arr + displs[rank], rcounts[rank] * sizeof(int));

	oddEven(localArr, rcounts[rank]);

	for (int i = 0; i < size; i++) {
        if (i % 2 == 1) {
            if (rank % 2 == 1) {
                if (rank < size - 1) {
                    int *buf = (int *)malloc(rcounts[rank + 1] * sizeof(int));
                    MPI_Recv(buf, rcounts[rank + 1], MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &st);
                    split(localArr, rcounts[rank], buf, rcounts[rank + 1]);
                    MPI_Send(buf, rcounts[rank + 1], MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                    free(buf);
                }
            } else if (rank > 0) {
                MPI_Sendrecv(localArr, rcounts[rank], MPI_INT, rank - 1, 0, localArr, rcounts[rank], MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &st);
            }
        } else {
            if(rank % 2 == 0) {
                if (rank < size - 1) {
                    int *buf = (int *)malloc(rcounts[rank + 1] * sizeof(int));
                    MPI_Recv(buf, rcounts[rank + 1], MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &st);
                    split(localArr, rcounts[rank], buf, rcounts[rank + 1]);
                    MPI_Send(buf, rcounts[rank + 1], MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                    free(buf);
                }
            } else {
                MPI_Sendrecv(localArr, rcounts[rank], MPI_INT, rank - 1, 0, localArr, rcounts[rank], MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &st);
            }
        }
    }

	MPI_Gatherv(localArr, rcounts[rank], MPI_INT, arr, rcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Hey, Buddy, you choose the wrong door, the parallel club has two args of command line\n");
		return 0;
	}

	MPI_Init(&argc, &argv);

    int N = atoi(argv[1]);
	// filling arr with random numbers
	int *arr = (int *)calloc(N, sizeof(int));
	fillRand(arr, N);

	parallelOddEven(arr, N);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (!rank)
		printf("%d\n", verify(arr, N));

	free(arr);
	MPI_Finalize();
	return 0;
}
