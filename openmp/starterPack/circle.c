#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int value = 0, i;

	#pragma omp parallel for ordered
	for (i = 0; i < omp_get_num_threads(); ++i) {
        #pragma omp ordered
	    printf("var = %d in thread number: %d\n", value++, omp_get_thread_num());
	}

	return 0;
}
