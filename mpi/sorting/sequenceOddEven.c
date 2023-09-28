#include <stdio.h>
#include <stdlib.h>
#include "oddEvenBase.h"

int main (int argc, char *argv[]) {
	if (argc != 2) {
		printf("Hey, Buddy, you choose the wrong door, the parallel club has two args of command line\n");
		return 0;
	}

    int size = atoi(argv[1]);
    int *arr = (int *)calloc(size, sizeof(int));
	fillRand(arr, size);

	oddEven(arr, size);
	
	printf("%d\n", verify(arr, size));

	free(arr);

	return 0;
}
