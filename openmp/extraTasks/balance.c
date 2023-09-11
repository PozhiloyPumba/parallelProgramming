#include <stdio.h>
#include <omp.h>

#define NUM_ITERS   65
#define NUM_THREADS 4

// #define SCHED_STATIC
// #define SCHED_DYNAMIC
// #define SCHED_GUIDED

#define CHUNK_SIZE 4

#ifdef SCHED_STATIC
    #define PARAMETRS schedule(static, CHUNK_SIZE)
#elif defined(SCHED_DYNAMIC)
    #define PARAMETRS schedule(dynamic, CHUNK_SIZE)
#elif defined(SCHED_GUIDED)
    #define PARAMETRS schedule(guided, CHUNK_SIZE)
#else
    #define PARAMETRS
#endif

int main(int argc, char* argv[])
{
    omp_set_dynamic(0);
    omp_set_num_threads(NUM_THREADS);

    #pragma omp parallel for PARAMETRS
    for(int i = 0; i < NUM_ITERS; i++) {
        printf("%d %d\n", omp_get_thread_num(), i);
    }

    return 0;
}