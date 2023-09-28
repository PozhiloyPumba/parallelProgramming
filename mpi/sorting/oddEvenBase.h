#ifndef ODD_EVEN_BASE_H
#define ODD_EVEN_BASE_H

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define MAX 1000000

void fillRand(int *arr, int N);
int verify(const int *arr, const size_t n);
void swap (int *first, int *second);
void oddEven(int *arr, int size);

#endif
