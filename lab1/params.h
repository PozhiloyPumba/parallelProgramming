#include <math.h>

const double a = 2.0;
const double T = 1.0, X = 1.0;

const int K = 201, M = 201;

const double tau = T / (K - 1), h = X / (M - 1);

#define X_VALUE(index) (h * (index))
#define T_VALUE(index) (tau * (index))

double phi(double x) {
	return cos(3.141592 * x);
}

double psi(double t) {
	return exp(-t);
}

double f(double x, double t) {
	return x + t;
}
