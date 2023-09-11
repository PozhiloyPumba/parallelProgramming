#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int a = 0;
#pragma omp threadprivate(a)

int main(int argc, char* argv[])
{
	omp_set_dynamic(0);

	#pragma omp parallel copyin(a)
	{
		#pragma omp master
		{
			printf("[First] Master thread changes the value of a to 42.\n");
			a = 42;
		}
		printf("[First] Thread %d: a = %d.\n", omp_get_thread_num(), a);
	}

    // update in all threads value from master
	#pragma omp parallel copyin(a)
	{
		printf("[Second] Thread %d: a = %d.\n", omp_get_thread_num(), a);
	}

	return EXIT_SUCCESS;
}