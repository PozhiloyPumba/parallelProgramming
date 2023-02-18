#include <stdio.h>
#include <math.h>
#include <gmp.h>
#include <mpi.h>

int countBorder (long long int length);
int heapCounts(int length);
unsigned long long distributionBorders (int length, int commRank);
void helper (int border);
void countE (int argc, char *argv[], mpf_t result);
