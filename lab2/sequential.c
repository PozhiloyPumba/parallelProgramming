#include <stdio.h>
#include <math.h>
#include "params.h"

int size = 0;

struct data {
	double left, right, fLeft, fRight, s;
} stk[1000];

void pushIntoStack (const struct data *in) {
	stk[size++] = *in;
}

void popFromStack (struct data *inout) {
	*inout = stk[--size];
}

int stackIsEmpty () {
	return !size;
}

void fillData (struct data *inout, double left, double right, double fLeft, double fRight, double s) { 
	struct data tmp = {left, right, fLeft, fRight, s};
	*inout = tmp;
}

double integrate(double (*f)(double), double left, double right, double epsilon) {
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
			pushIntoStack(&tmp);

			left = mid;
			fLeft = fMid;
			sLR = sMR;
		}
		else {
			value += sLMR;
			if (stackIsEmpty())
				break;
			popFromStack(&tmp);
			left = tmp.left;
			right = tmp.right;
			fLeft = tmp.fLeft;
			fRight = tmp.fRight;
			sLR = tmp.s;
		}
	}
	return value;
}

int main () {
	printf("%lf\n", integrate(func, A, B, EPSILON));
	return 0;
}
