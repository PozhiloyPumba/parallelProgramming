#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#ifndef NTHREADS
#define NTHREADS 10
#endif

struct thread_args {
	int myThreadId;
};

void *hello(void *inParams) {
	struct thread_args *p = (struct thread_args *)inParams;
	int id = p->myThreadId;

	printf("hello world from thread number %d, Number of threads %d\n", id, NTHREADS);
	free(inParams);
	return NULL;
}

int main() {
	pthread_t thread_id[NTHREADS];

	struct thread_args tmp;

	for(int i = 0; i < NTHREADS; ++i) {
		struct thread_args *p = (struct thread_args *) malloc(sizeof(struct thread_args)); 
		p->myThreadId = i;
		pthread_create(&thread_id[i], NULL, hello, (void *)p);
	}

	#ifdef HAVE_THR_SETCONCURRENCY_PROTO
	thr_setconcurrency(NTHREADS);
	#endif

	for(int i = 0; i < NTHREADS; ++i) {
		pthread_join(thread_id[i], NULL);
	}

	return 0;
}
