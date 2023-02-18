#include "countEuler.h"
#include <limits.h>
#include <stdlib.h>

int countBorder (long long int numOfDigits) {
	double border = numOfDigits;
	const int numOfIterations = 5;
	const double freeCoef = 0.9189385332 - log(10) * numOfDigits;
	double logBorder;

	for (int i = 0; i < numOfIterations; ++i) {
		logBorder = log(border);
		border -= ((logBorder - 1) * border + 1 / 2 * logBorder + freeCoef) / (logBorder + 1 / 2 / border);
	}

	//printf("count = %lg\n", border);

	return border + 3; // +3 for solution rounding problem
}

void helper (int border) {
	int heapCounter = 0;
	int heapSize = 1000;

	for(int i = 1; i < border;) {
		unsigned long long tmp = 1;
		int curHeapSize = 0;
		while (ULLONG_MAX / i > tmp) {
			++curHeapSize;
			tmp *= i++;
		}
		if (curHeapSize < heapSize) {
			heapSize = curHeapSize;
			printf("heapSize = %d on %d, heap count = %d\n", curHeapSize, i - 1 - curHeapSize, heapCounter);
		}
		++heapCounter;
		//printf("heap number = %d, border = %d\n", ++heapCounter, i);
	}
	printf("heap count = %d\n", heapCounter);

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

unsigned getIndexBorderFromHeap (int heapNumber, int length) {	
	int borders[] = {0, 20, 33, 45, 56, 86, 140, 252, 567, 1623, 7133, 65537, 2642246};
	int heapSizes[] = {20, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2};
	int heapCounts[] = {0, 1, 2, 3, 4, 7, 13, 27, 72, 248, 1350, 15951, 874854};

	int delimeterSizeMinusOne = sizeof(borders) / sizeof(int) - 1;
	int numOfBorder = -1;

	while (numOfBorder < delimeterSizeMinusOne && heapCounts[++numOfBorder] < heapNumber) 
		;
	unsigned num = (unsigned)(borders[numOfBorder] + (heapNumber - heapCounts[numOfBorder]) * heapSizes[numOfBorder - 1]);

	return length < num ? (unsigned)(length) : num;
}

#define LEFT_BORDER(x) ((x) >> 32)
#define RIGHT_BORDER(x) ((x) & 0x00000000FFFFFFFF) 
// this experimental values don't try understand this move
unsigned long long distributionBorders (int length, int commRank) {
	int commSize;
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
	int inverseRank = commSize - commRank - 1;

	int countHeaps = heapCounts(length);
	//printf("heaps = %d, length = %d\n", countHeaps, length);
	unsigned long long borders = 0;
	
	if (countHeaps < commSize) {
		unsigned long long left = (length * inverseRank) / commSize;
		borders |= left << 32;
		borders |= (length * (inverseRank + 1)) / commSize;
		//printf("rank = %d; left = %lu, right = %lu\n", commRank, LEFT_BORDER(borders), RIGHT_BORDER(borders));
		return borders;
	}

	int leftHeap = (countHeaps * inverseRank) / commSize,
		rightHeap = (countHeaps * (inverseRank + 1)) / commSize;

	unsigned long long left = getIndexBorderFromHeap (leftHeap, length);
	//printf("left = %lu\n", left);
	borders |= left << 32;
	borders |= getIndexBorderFromHeap (rightHeap, length);
	//printf("rank = %d; left = %lu, right = %lu\n", commRank, LEFT_BORDER(borders), RIGHT_BORDER(borders));

	return borders;
}

void mpz_set_ull(mpz_t n, unsigned long long ull)
{
    mpz_set_ui(n, (unsigned int)(ull >> 32));
	mpz_mul_2exp(n, n, 32);
	mpz_add_ui(n, n, (unsigned int)ull);
}

void countE (int argc, char *argv[], mpf_t result) {
    int commRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &commRank);
	int commSize;
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
	
	long long int precision = atoll(argv[1]);
	unsigned long long borders = distributionBorders(countBorder(precision), commRank);

	mpz_t sum, prod;
	mpz_init(sum);
	mpz_init_set_ui (prod, 1);

	for(unsigned i = RIGHT_BORDER(borders), end = LEFT_BORDER(borders); i > end; --i) {
		mpz_mul_ui (prod, prod, i);
		mpz_add (sum, sum, prod);
	}

	//gmp_printf("rank %d; prod = %Zd; sum = %Zd\n", commRank, prod, sum);
	
	mpz_t fact; mpz_init(fact);

	if (commRank != commSize - 1) {
		size_t szProd = mpz_sizeinbase(prod, 10);

		char *buf = (char *)malloc(szProd);
		if (!buf) return;

		gmp_sprintf (buf, "%Zd", prod);
		for(int i = commRank + 1; i < commSize; ++i) {
			MPI_Send(&szProd,  1,  MPI_INT,  i, 0, MPI_COMM_WORLD);
			MPI_Send(buf, szProd, MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
		free(buf);
	}
	else {
		mpz_add(fact, fact, prod);
	}

	for(int i = 0; i < commRank; ++i) {
		int szProd;
		MPI_Status status;
		MPI_Recv(&szProd,  1,  MPI_INT, i, 0, MPI_COMM_WORLD, &status);
		char *buf = (char *)malloc(szProd);

		MPI_Recv(buf, szProd, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
		gmp_sscanf(buf, "%Zd", &prod);
		free(buf);
		mpz_mul(sum, sum, prod);
		if (commRank == commSize - 1) mpz_mul(fact, fact, prod);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	
	//gmp_printf("rank %d; fact = %Zd\n", commRank, fact);
	
	if (commRank) {
		size_t szSum = mpz_sizeinbase(sum, 10);

		char *buf = (char *)malloc(szSum);
		gmp_sprintf (buf, "%Zd", sum);

		MPI_Send(&szSum,  1,  MPI_INT,  0, 0, MPI_COMM_WORLD);
		MPI_Send(buf, szSum, MPI_CHAR,  0, 0, MPI_COMM_WORLD);

		free(buf);
	}
	else {
		for(int i = 1; i < commSize; ++i) {
			int szSum;
			MPI_Status status;

			MPI_Recv(&szSum,  1,  MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			char *buf = (char *)malloc(szSum);

			MPI_Recv(buf, szSum, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
			gmp_sscanf(buf, "%Zd", &prod);
			free(buf);
			mpz_add(sum, sum, prod);
		}
	}
	//gmp_printf("rank %d; sum = %Zd\n", commRank, sum);
	
	if (commRank == commSize - 1) {
		int szFact = mpz_sizeinbase(fact, 10);
		MPI_Status status;
		MPI_Send(&szFact,  1,  MPI_INT, 0, 0, MPI_COMM_WORLD);
		char *buf = (char *)malloc(szFact);
		gmp_sprintf(buf, "%Zd", fact);

		MPI_Send(buf, szFact, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
		free(buf);
	}

	if (!commRank) {
		int szProd;
		MPI_Status status;
		MPI_Recv(&szProd,  1,  MPI_INT, commSize - 1, 0, MPI_COMM_WORLD, &status);
		char *buf = (char *)malloc(szProd);

		MPI_Recv(buf, szProd, MPI_CHAR, commSize - 1, 0, MPI_COMM_WORLD, &status);
		
		gmp_sscanf(buf, "%Zd", &prod);
		mpf_t factorial; mpf_init(factorial);
		mpf_set_prec(factorial, 10000);
		mpf_set_z(factorial, prod);

		free(buf);
		
		//gmp_printf("rank = %d, sum = %Zd\n fact = %Ff\n", commRank, sum, factorial);
	
		mpz_add_ui(sum, sum, 1);
		mpf_set_z(result, sum);
		mpf_set_prec(result, 10000);

		//gmp_printf("rank = %d, sum = %Zd\n fact = %Ff\n", commRank, sum, factorial);
		
		mpf_div (result, result, factorial);

		gmp_printf("ans = %.50Ff\n", result);
	}
}
