#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char* argv[])
{
	int a = 0;
	#pragma omp parallel firstprivate(a)
	{
		printf("Thread %d: a = %d.\n", omp_get_thread_num(), a);
		#pragma omp barrier

		#pragma omp single copyprivate(a)
		{
			a = 42;
			printf("Thread %d in single and changes a to %d.\n", omp_get_thread_num(), a);
		}

		printf("Thread %d: a = %d.\n", omp_get_thread_num(), a);
	}

	return 0;
}