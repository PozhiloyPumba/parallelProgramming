#include <gmp.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>

int countBorder(long long int length);
int heapCounts(int length);
unsigned long long distributionBorders(int length, int commRank);
void helper(int border);
void countE(int argc, char *argv[], mpf_t result);
