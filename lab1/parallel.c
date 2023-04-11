#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "params.h"

// K -> T
// M -> X


int main(int argc, char *argv[]) {
	int size, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status st;

	int begin = M / size * rank, end = (rank == size - 1) ? M : M / size * (rank + 1);

	double *answer;
	if (!rank) {
		answer = (double *)calloc(K * M, sizeof(double));

		for (int i = begin; i < end; ++i)
			answer[K * i] = phi(X_VALUE(i));
		for (int i = 0; i < K; ++i)
			answer[i] = psi(T_VALUE(i));

		for (int i = 1; i < K; ++i) {
			for (int j = begin + 1; j < end; ++j) {
				answer[j * K + i] = (f(h * ((double)j - 0.5), tau * ((double)i - 0.5)) * tau * h * 2 - (answer[K * j + i - 1] - answer[i + K * (j - 1)]) * (a * tau - h)) / (h + a * tau) + answer[i - 1 + K * (j - 1)];
			}
			if (rank + 1 < size) MPI_Send(answer + K * (end - 1) + i, 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);	// send to next
		}
	} else {
		answer = (double *)calloc(K * (end - begin + 1), sizeof(double));
		int delta = end - begin;

		for (int i = begin - 1; i < end; ++i)
			answer[K * (i - begin + 1)] = phi(X_VALUE(i));

		for (int i = 1; i < K; ++i) {
			MPI_Recv(answer + i, 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &st);	// get from prev
			for (int j = 1; j < delta + 1; ++j) {
				answer[j * K + i] = (f(h * ((double)(j + begin - 1) - 0.5), tau * ((double)i - 0.5)) * tau * h * 2 - (answer[K * j + i - 1] - answer[i + K * (j - 1)]) * (a * tau - h)) / (h + a * tau) + answer[i - 1 + K * (j - 1)];
			}
			if (rank != size - 1) MPI_Send(answer + K * (delta) + i, 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);	 // send to next
		}
	}

	int *displs = (int *)malloc(size * sizeof(int));
	int *rcounts = (int *)malloc(size * sizeof(int));
	for (int i = 0; i < size; ++i) {
		int locBegin = M / size * i;
		int locEnd = (i == size - 1) ? M : M / size * (i + 1);
		rcounts[i] = K * (locEnd - locBegin);
		displs[i] = (i == 0) ? 0 : displs[i - 1] + rcounts[i - 1];
	}

	MPI_Gatherv(answer + K * (!!rank), rcounts[rank], MPI_DOUBLE, answer, rcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (!rank) {
		printf("%lf %d %lf %d\n", T, K, X, M);
		for (int i = 0; i < K; ++i) {
			for (int j = 0; j < M; ++j) {
				printf("%lf ", answer[j * K + i]);
			}
			printf("\n");
		}
	}

	free(answer);

	MPI_Finalize();
	return 0;
}
