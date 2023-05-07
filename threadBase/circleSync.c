#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#ifndef NTHREADS
#define NTHREADS 10
#endif

struct thread_args {
	int threadNum;
};

int glob = 0, globId = 0;
pthread_mutex_t globSync;

void *circle(void *inParams) {
	struct thread_args *p = (struct thread_args *)inParams;
	int id = p->threadNum;
	free(inParams);

	while (id != globId);

	pthread_mutex_lock(&globSync);
	printf("thread id = %d, glob = %d\n", id, ++glob);
	++globId;
	pthread_mutex_unlock(&globSync);

	return NULL;
}

int main() {
	pthread_t thread_id[NTHREADS];
	pthread_mutex_init(&globSync, NULL);

	for(int i = 0; i < NTHREADS; ++i) {
		struct thread_args *p = (struct thread_args *) malloc(sizeof(struct thread_args)); 
		p->threadNum = i;
		pthread_create(&thread_id[i], NULL, circle, (void *)p);
	}

	#ifdef HAVE_THR_SETCONCURRENCY_PROTO
	thr_setconcurrency(NTHREADS);
	#endif

	for(int i = 0; i < NTHREADS; ++i) {
		pthread_join(thread_id[i], NULL);
	}

	pthread_mutex_destroy(&globSync);

	return 0;
}
