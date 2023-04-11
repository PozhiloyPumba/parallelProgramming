#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000000

int main (int argc, char *argv[]) {
	srand(time(NULL));
    int size = atoi(argv[1]);
    int arr = (int *)calloc(, sizeof(int));
 
    for (int i = 0; i < size; ++i) {
        arr[i] = rand() % MAX;
	}
}
