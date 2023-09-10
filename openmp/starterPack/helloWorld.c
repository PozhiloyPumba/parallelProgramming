#include <omp.h>
 
#include <stdio.h>
#include <stdlib.h>
 
int main(int argc, char* argv[])
{
    #pragma omp parallel
    {
        printf("Hello World... from thread = %d, count threads = %d\n",
               omp_get_thread_num(), omp_get_num_threads());
    }
}