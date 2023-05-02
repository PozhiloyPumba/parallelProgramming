#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>

#define NTHREADS 10

int size = 0;

struct thread_args {
	double (*f)(double);
	double left, right, epsilon;	
};

struct data {
	double left, right, fLeft, fRight, s;
};

struct stack {
	int size;
	struct data stk[1000];
};

struct stack gloabalStack;

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

double func (double x) {
	return sin(1/x);
}

void *integrate(void *inParams) {
	struct thread_args *p = (struct thread_args *)inParams;
	double (*f)(double) = p->f;
	double left    = p->left,
		   right   = p->right,
		   epsilon = p->epsilon;

	struct stack s = {};

	double value = 0;
	double fLeft = f(left), fRight = f(right);
	double sLR = (fLeft + fRight) * (right - left) / 2;
	struct data tmp;

	while (1) {
		double mid = (left + right) / 2;
		double fMid = f(mid);

		double	sLM  = (fLeft + fMid) * (mid - left) / 2,
				sMR  = (fMid + fRight) * (right - mid) / 2;
		double  sLMR = sLM + sMR;

		if(fabs(sLR-sLMR) > epsilon * fabs(sLMR)) {
			fillData(&tmp, left, mid, fLeft, fMid, sLM);
			pushIntoStack(&s, &tmp);

			left = mid;
			fLeft = fMid;
			sLR = sMR;
		}
		else {
			value += sLMR;
			if (stackIsEmpty(&s))
				break;
			popFromStack(&s, &tmp);
			left = tmp.left;
			right = tmp.right;
			fLeft = tmp.fLeft;
			fRight = tmp.fRight;
			sLR = tmp.s;
		}	
	}
	free(inParams);
	double *ret = (double *) malloc(1 * sizeof(double));
	*ret = value;
	return ret;
}

int main() {
	pthread_t thread_id[NTHREADS];
	double answer = 0;
	double A = 0.01, B = 50;

	for(int i = 0; i < NTHREADS; ++i) {
		struct thread_args *p = (struct thread_args *) malloc(sizeof(struct thread_args)); 
		struct thread_args tmp = {func, A + (B - A) * i / NTHREADS, A + (B - A) * (i + 1) / NTHREADS, 0.0001 / NTHREADS};
		*p = tmp;
		pthread_create(&thread_id[i], NULL, integrate, (void *)p);
	}

	for(int i = 0; i < NTHREADS; ++i) {
		double *localResult;
		pthread_join(thread_id[i], (void **)&localResult);
		answer += *(localResult);
		free(localResult);
	}
	
	printf("%lf\n", answer);
	return 0;
}
