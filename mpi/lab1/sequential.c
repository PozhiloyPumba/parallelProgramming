#include <stdio.h>
#include <stdlib.h>

#include "params.h"

void setBorderConditions(double **answer) {
	for (int i = 0; i < M; ++i) {
		answer[0][i] = phi(X_VALUE(i));
	}
	for (int i = 0; i < K; ++i) {
		answer[i][0] = psi(T_VALUE(i));
	}
}

void solve(double **answer) {
	setBorderConditions(answer);

	for (int i = 1; i < K; ++i) {
		for (int j = 1; j < M; ++j) {
			answer[i][j] = (f(h * ((double)j - 0.5), tau * ((double)i - 0.5)) * tau * h * 2 - (answer[i - 1][j] - answer[i][j - 1]) * (a * tau - h)) / (h + a * tau) + answer[i - 1][j - 1];
		}
	}
}

int main() {
	double *answer = (double *)calloc(K * M, sizeof(double));

	for (int i = 0; i < M; ++i)
		answer[K * i] = phi(X_VALUE(i));
	for (int i = 0; i < K; ++i)
		answer[i] = psi(T_VALUE(i));

	for (int i = 1; i < K; ++i) {
		for (int j = 1; j < M; ++j) {
			answer[j * K + i] = (f(h * ((double)j - 0.5), tau * ((double)i - 0.5)) * tau * h * 2 - (answer[K * j + i - 1] - answer[i + K * (j - 1)]) * (a * tau - h)) / (h + a * tau) + answer[i - 1 + K * (j - 1)];
		}
	}
#ifndef QUIET
	printf("%lf %d %lf %d\n", T, K, X, M);
	for (int i = 0; i < K; ++i) {
		for (int j = 0; j < M; ++j) {
			printf("%lf ", answer[j * K + i]);
		}
		printf("\n");
	}
#endif
/*
	double **answer = (double **)calloc(K, sizeof(double *));
	for (int i = 0; i < K; ++i) {
		answer[i] = (double *)calloc(M, sizeof(double));
	}

	solve(answer);

#ifndef QUIET 
	printf("%lf %d %lf %d\n", T, K, X, M);
	for (int i = 0; i < K; ++i) {
		for (int j = 0; j < M; ++j) {
			printf("%lf ", answer[i][j]);
		}
		printf("\n");
	}
#endif

	for (int i = 0; i < K; ++i) {
		free(answer[i]);
	}
*/
	free(answer);

	return 0;
}
