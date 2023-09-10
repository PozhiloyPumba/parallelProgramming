#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

// #define REDUCTION
#define MASTER

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Hey, Buddy, you choose the wrong door, the parallel club has two args of command line\n");
		return 0;
	}

	int num = atoi(argv[1]);

	double sum = 0.;
	int i;
#if defined(REDUCTION)
	#pragma omp parallel for reduction (+:sum)
	for (i = 1; i < num + 1; ++i) {
		sum += 1.0 / i;
	}
#elif defined(MASTER)
	double *partialSum = (double *) calloc(omp_get_max_threads(), sizeof(double));
	#pragma omp parallel for
	for (i = 1; i < num + 1; ++i) {
		partialSum[omp_get_thread_num()] += 1.0 / i;
	}

	for (int j = 0, numThreads = omp_get_max_threads(); j < numThreads; ++j) {
		sum += partialSum[j];
	}
	free(partialSum);
#endif
	printf("sum = %lg\n", sum);

	return 0;
}
