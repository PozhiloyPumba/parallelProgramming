#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#ifndef NTHREADS
#define NTHREADS 10
#endif

struct thread_args {
	int left, right;
};

void *partialSum(void *inParams) {
	struct thread_args *p = (struct thread_args *)inParams;
	double left = p->left, right = p->right;
	free(inParams);
	double *ret = (double *) malloc(1 * sizeof(double));
	*ret = 0;

	for (int i = left; i < right; ++i) {
		*ret += 1.0 / i;
	}
	return ret;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Hey, Buddy, you choose the wrong door, the parallel club has two args of command line\n");
		return 0;
	}

	pthread_t thread_id[NTHREADS];
	int num = atoi(argv[1]);
	double answer = 0;

	for(int i = 0; i < NTHREADS; ++i) {
		struct thread_args *p = (struct thread_args *) malloc(sizeof(struct thread_args)); 
		p->left = num / NTHREADS * i + 1;
		p->right = (i == NTHREADS - 1) ? num + 1 : num / NTHREADS * (i + 1) + 1;
		pthread_create(&thread_id[i], NULL, partialSum, (void *)p);
	}

	#ifdef HAVE_THR_SETCONCURRENCY_PROTO
	thr_setconcurrency(NTHREADS);
	#endif

	for(int i = 0; i < NTHREADS; ++i) {
		double *localResult;
		pthread_join(thread_id[i], (void **)&localResult);
		answer += *localResult;
		free(localResult);
	}

	printf("%lg\n", answer);
	return 0;
}
