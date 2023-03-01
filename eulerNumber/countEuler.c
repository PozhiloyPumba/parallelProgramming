#include "countEuler.h"

#include <limits.h>
#include <stdlib.h>

int countBorder(long long int numOfDigits) {
	double border = numOfDigits;
	const int numOfIterations = 5;
	const double freeCoef = 0.9189385332 - log(10) * numOfDigits;
	double logBorder;

	for (int i = 0; i < numOfIterations; ++i) {
		logBorder = log(border);
		border -= ((logBorder - 1) * border + 1 / 2 * logBorder + freeCoef) / (logBorder + 1 / 2 / border);
	}

	return border + 3;	// +3 for solution rounding problem
}

int heapCounts(int length) {
	int borders[] = {0, 20, 33, 45, 56, 86, 140, 252, 567, 1623, 7133, 65537, 2642246};
	int heapSizes[] = {20, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2};
	int heapCounts[] = {0, 1, 2, 3, 4, 7, 13, 27, 72, 248, 1350, 15951, 874854};

	int delimeterSizeMinusOne = sizeof(borders) / sizeof(int) - 1;
	int numOfBorder = -1;

	while (numOfBorder < delimeterSizeMinusOne && borders[++numOfBorder] < length)
		;
	--numOfBorder;

	return heapCounts[numOfBorder] + (length - borders[numOfBorder]) / heapSizes[numOfBorder] + 1;
}

unsigned getIndexBorderFromHeap(int heapNumber, int length) {
	int borders[] = {0, 20, 33, 45, 56, 86, 140, 252, 567, 1623, 7133, 65537, 2642246};
	int heapSizes[] = {20, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2};
	int heapCounts[] = {0, 1, 2, 3, 4, 7, 13, 27, 72, 248, 1350, 15951, 874854};

	int delimeterSizeMinusOne = sizeof(borders) / sizeof(int) - 1;
	int numOfBorder = -1;

	while (numOfBorder < delimeterSizeMinusOne && heapCounts[++numOfBorder] < heapNumber)
		;
	int leftNumBorder = numOfBorder ? numOfBorder - 1 : 0;
	unsigned num = (unsigned)(borders[numOfBorder] + (heapNumber - heapCounts[numOfBorder]) * heapSizes[leftNumBorder]);

	return length < num ? (unsigned)(length) : num;
}

#define LEFT_BORDER(x) ((x) >> 32)
#define RIGHT_BORDER(x) ((x)&0x00000000FFFFFFFF)
// this experimental values don't try to understand this move
unsigned long long distributionBorders(int length, int commRank) {
	int commSize;
	MPI_Comm_size(MPI_COMM_WORLD, &commSize);

	int countHeaps = heapCounts(length);
	unsigned long long borders = 0;

	if (countHeaps < commSize) {
		unsigned long long left = (length * commRank) / commSize;
		borders |= left << 32;
		borders |= (length * (commRank + 1)) / commSize;
		return borders;
	}

	int leftHeap = (countHeaps * commRank) / commSize,
		rightHeap = (countHeaps * (commRank + 1)) / commSize;

	unsigned long long left = getIndexBorderFromHeap(leftHeap, length);
	borders |= left << 32;
	borders |= getIndexBorderFromHeap(rightHeap, length);

	return borders;
}

int distributionHeapBorders(int length, int commRank) {
	int commSize;
	MPI_Comm_size(MPI_COMM_WORLD, &commSize);

	int countHeaps = heapCounts(length);
	unsigned long long borders = 0;

	if (countHeaps < commSize)
		return 1;

	int leftHeap = (countHeaps * commRank) / commSize,
		rightHeap = (countHeaps * (commRank + 1)) / commSize;

	return rightHeap - leftHeap ? rightHeap - leftHeap : 1;
}

void mpz_set_ull(mpz_t n, unsigned long long ull) {
	mpz_set_ui(n, (unsigned int)(ull >> 32));
	mpz_mul_2exp(n, n, 32);
	mpz_add_ui(n, n, (unsigned int)ull);
}

void countE(int argc, char *argv[], mpf_t result) {
	int commRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &commRank);
	int commSize;
	MPI_Comm_size(MPI_COMM_WORLD, &commSize);

	long long int precision = atoll(argv[1]);
	int length = countBorder(precision);
	int lenHeap = distributionHeapBorders(length, commRank);	
	unsigned long long borders = distributionBorders(length, commRank);

	mpz_t sum, prod;
	mpz_init(sum);
	mpz_init_set_ui(prod, 1);
#ifdef ADDITIONAL_MEMORY
	mpz_t *sums = malloc(sizeof(mpz_t) * lenHeap);
	mpz_t *prods = malloc(sizeof(mpz_t) * lenHeap);

	int jLen = 0;
	for (unsigned i = RIGHT_BORDER(borders), end = LEFT_BORDER(borders); i > end;) {
		unsigned long long tmpProd = 1, tmpSum = 0;
		while (i > end && ULLONG_MAX / i > tmpProd) {
			tmpProd *= i--;
			tmpSum += tmpProd;
		}

		mpz_inits(sums[jLen], prods[jLen], NULL);
		mpz_set_ull(sums[jLen], tmpSum);
		mpz_set_ull(prods[jLen], tmpProd);
		++jLen;
	}

	lenHeap = jLen;
	for (unsigned i = 2; i < 2 * lenHeap; i *= 2) {
		unsigned stepToNeighbour = i / 2;
		for (unsigned j = 0; j + stepToNeighbour < lenHeap; j += i) {
			mpz_addmul(sums[j], prods[j], sums[j + stepToNeighbour]);
			mpz_mul(prods[j], prods[j], prods[j + stepToNeighbour]);
		}
	}
	if (mpz_sgn (prods[0]))
		mpz_swap(prod, prods[0]);
	mpz_swap(sum, sums[0]);
	for (int i = 0; i < lenHeap; ++i)
		mpz_clears(sums[i], prods[i], NULL);

	free(sums);
	free(prods);
#else
	for(unsigned i = RIGHT_BORDER(borders), end = LEFT_BORDER(borders); i > end; --i) {
		unsigned long long tmpProd = 1, tmpSum = 0;
		while (i > end && ULLONG_MAX / i > tmpProd) {
			tmpProd *= i--;
			tmpSum += tmpProd;
		}
		++i;

		mpz_t longTmpSum, longTmpProd; mpz_init(longTmpSum); mpz_init(longTmpProd);
		mpz_set_ull(longTmpSum, tmpSum);
		mpz_set_ull(longTmpProd, tmpProd);

		mpz_addmul(sum, prod, longTmpSum);
		mpz_mul(prod, prod, longTmpProd);
	}
#endif
	mpz_t fact;
	mpz_init(fact);

	if (commRank) {
		size_t szProd = mpz_sizeinbase(prod, 10) + 1;

		char *buf = (char *)calloc(szProd, sizeof(char));
		if (!buf) MPI_Abort(MPI_COMM_WORLD, 911);

		gmp_sprintf(buf, "%Zd", prod);
		for (int i = commRank - 1; i >= 0; --i) {
			MPI_Send(&szProd, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(buf, szProd, MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
		free(buf);
	} else {
		mpz_add(fact, fact, prod);
	}

	for (int i = commRank + 1; i < commSize; ++i) {
		int szProd;
		MPI_Status status;
		MPI_Recv(&szProd, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
		char *buf = (char *)calloc(szProd, sizeof(char));
		if (!buf) MPI_Abort(MPI_COMM_WORLD, 911);

		MPI_Recv(buf, szProd, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
		gmp_sscanf(buf, "%Zd", &prod);

		free(buf);
		mpz_mul(sum, sum, prod);
		if (!commRank) mpz_mul(fact, fact, prod);
	}

	if (commRank) {
		size_t szSum = mpz_sizeinbase(sum, 10) + 1;

		char *buf = (char *)calloc(szSum, sizeof(char));

		if (!buf) MPI_Abort(MPI_COMM_WORLD, 911);

		gmp_sprintf(buf, "%Zd", sum);

		MPI_Send(&szSum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send(buf, szSum, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

		free(buf);
	} else {
		for (int i = 1; i < commSize; ++i) {
			int szSum;
			MPI_Status status;

			MPI_Recv(&szSum, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			char *buf = (char *)calloc(szSum, sizeof(char));
			if (!buf) MPI_Abort(MPI_COMM_WORLD, 911);

			MPI_Recv(buf, szSum, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
			gmp_sscanf(buf, "%Zd", &prod);
			free(buf);
			mpz_add(sum, sum, prod);
		}
	}

	if (!commRank) {
		mpf_t factorial;
		mpf_init(factorial);
		mpf_set_prec(factorial, 8 * precision);
		mpf_set_z(factorial, fact);

		mpz_add_ui(sum, sum, 1);
		mpf_set_prec(result, 8 * precision);
		mpf_set_z(result, sum);

		mpf_div(result, result, factorial);

		mpf_clear(factorial);
	}
	mpz_clears(sum, prod, fact, NULL);
}
