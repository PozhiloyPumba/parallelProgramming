#include "oddEvenBase.h"

void fillRand(int *arr, int N) {
	srand(time(NULL));

    for (int i = 0; i < N; ++i) {
        arr[i] = rand() % MAX;
	}
}

int verify(const int *arr, const size_t n) {
    for (size_t i = 1; i < n; ++i) {
        if (arr[i] < arr[i - 1]) {
            return 0;
        }
    }
    return 1;
}

void swap (int *first, int *second) {
    int tmp = *first;
    *first  = *second;
    *second = tmp;
}

void oddEven(int *arr, int size) {
	for (size_t i = 0; i < size; i++) {
		for (size_t j = (i % 2) ? 0 : 1; j + 1 < size; j += 2) {
			if (arr[j] > arr[j + 1]) {
				swap(arr + j + 1, arr + j);
			}
		}
	}

	return;
}
