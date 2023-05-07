#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include "params.h"

#ifndef NTHREADS
#define NTHREADS 10
#endif

#ifndef IS_MNOGO
#define IS_MNOGO 5
#endif

int size = 0;

struct thread_args {
	double (*f)(double);
	double epsilon;	
};

struct globData {
	pthread_mutex_t globNotEmpty;
	pthread_mutex_t readGlob;
	int nActiveThreads;
};

struct globData sync;

struct data {
	double left, right, fLeft, fRight, s;
};

#define MAX_TASKS 1000

struct stack {
	int size;
	struct data stk[MAX_TASKS];
};

struct stack globalStack;

void pushIntoStack (struct stack *s, const struct data *in) {
	s->stk[s->size++] = *in;
}

void popFromStack (struct stack *s, struct data *inout) {
	*inout = s->stk[--s->size];
}

int stackIsEmpty (struct stack *s) {
	return !s->size;
}

void fillData (struct data *inout, double left, double right, double fLeft, double fRight, double s) { 
	struct data tmp = {left, right, fLeft, fRight, s};
	*inout = tmp;
}

void *integrate(void *inParams) {
	struct thread_args *p = (struct thread_args *)inParams;
	double (*f)(double) = p->f;
	double epsilon = p->epsilon;

	struct stack s = {};

	double value = 0;
	double left = 0, right = 0, fLeft = 0, fRight = 0;
	double sLR = 0;
	struct data tmp;

	while (1) {
		pthread_mutex_lock(&sync.globNotEmpty);
		pthread_mutex_lock(&sync.readGlob);

		popFromStack(&globalStack, &tmp);
		left = tmp.left;
		right = tmp.right;
		fLeft = tmp.fLeft;
		fRight = tmp.fRight;
		sLR = tmp.s;

		if (!stackIsEmpty(&globalStack))
			pthread_mutex_unlock(&sync.globNotEmpty);

		if (right >= left)
			++sync.nActiveThreads;

		pthread_mutex_unlock(&sync.readGlob);

		if (left > right)
			break;

		while (1) {
			double mid = (left + right) / 2;
			double fMid = f(mid);

			double	sLM  = (fLeft + fMid) * (mid - left) / 2,
					sMR  = (fMid + fRight) * (right - mid) / 2;
			double	sLMR = sLM + sMR;

			if(fabs(sLR-sLMR) > epsilon * fabs(sLMR)) {
				fillData(&tmp, left, mid, fLeft, fMid, sLM);
				pushIntoStack(&s, &tmp);

				left = mid;
				fLeft = fMid;
				sLR = sMR;
			}
			else {
				value += sLMR;
				if (stackIsEmpty(&s)) {
					break;
				}
				popFromStack(&s, &tmp);
				left = tmp.left;
				right = tmp.right;
				fLeft = tmp.fLeft;
				fRight = tmp.fRight;
				sLR = tmp.s;
			}

			if (s.size > IS_MNOGO && stackIsEmpty(&globalStack)) {
				pthread_mutex_lock(&sync.readGlob);
				
				if (stackIsEmpty(&globalStack))
					pthread_mutex_unlock(&sync.globNotEmpty);

				while (s.size > 1 && globalStack.size < MAX_TASKS - 1) {
					popFromStack(&s, &tmp);
					pushIntoStack(&globalStack, &tmp);
				}
				pthread_mutex_unlock(&sync.readGlob);
			}
		}

		pthread_mutex_lock(&sync.readGlob);

		--sync.nActiveThreads;
		if (!sync.nActiveThreads && stackIsEmpty(&globalStack)) {
			for(int i = 0; i < NTHREADS; ++i) {
				tmp.left = 2;
				tmp.right = 1;
				pushIntoStack(&globalStack, &tmp);
			}
			pthread_mutex_unlock(&sync.globNotEmpty);
		}
		pthread_mutex_unlock(&sync.readGlob);
	}
	free(inParams);
	double *ret = (double *) malloc(1 * sizeof(double));
	*ret = value;
	return ret;
}

int main() {
	pthread_t thread_id[NTHREADS];
	double answer = 0;

	struct data forGlobStk = {A, B, func(A), func(B), (func(A) + func(B)) * (B - A) / 2};
	pushIntoStack(&globalStack, &forGlobStk);

	pthread_mutex_init(&sync.globNotEmpty, NULL);
	pthread_mutex_init(&sync.readGlob, NULL);
	sync.nActiveThreads = 0;

	struct thread_args tmp = {func, EPSILON};

	for(int i = 0; i < NTHREADS; ++i) {
		struct thread_args *p = (struct thread_args *) malloc(sizeof(struct thread_args)); 
		*p = tmp;
		pthread_create(&thread_id[i], NULL, integrate, (void *)p);
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

	pthread_mutex_destroy(&sync.globNotEmpty);
	pthread_mutex_destroy(&sync.readGlob);

	printf("%lf\n", answer);
	return 0;
}
