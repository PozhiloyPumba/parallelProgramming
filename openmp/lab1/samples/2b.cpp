#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <omp.h>

#define CHECK_CORRECT
#include "common.hpp"


int main(int argc, char **argv) {
    std::vector<Type> a(ISIZE * JSIZE);

    for (int i = 0; i < ISIZE; ++i)
        for (int j = 0; j < JSIZE; ++j)
            a[i * JSIZE+ j] = 10 * i + j;

    #pragma omp barrier
    double exec_time = -omp_get_wtime();

    // D = {'<', '>'}
    // D_i = 1 stream not paralleling ((
    for (int i = 0; i < ISIZE - 1; ++i)
        // D_j = -6 but in other i then parallel for it index independent
        #pragma omp parallel for
        for (int j = 6; j < JSIZE; ++j)
           a[i * JSIZE + j] = sin(0.2 * a[(i + 1) * JSIZE + j - 6]);

    #pragma omp barrier
    exec_time += omp_get_wtime();

    std::cout << "parallel: " << exec_time << std::endl;

#ifdef CHECK_CORRECT
    std::vector<Type> a_true(ISIZE * JSIZE);
    
    for (int i = 0; i < ISIZE; ++i)
        for (int j = 0; j < JSIZE; ++j)
            a_true[i * JSIZE+ j] = 10 * i + j;

    #pragma omp barrier
    exec_time = -omp_get_wtime();

    for (int i = 0; i < ISIZE - 1; ++i)
        for (int j = 6; j < JSIZE; ++j)
            a_true[i * JSIZE + j] = sin(0.2 * a_true[(i + 1) * JSIZE + j - 6]);

    #pragma omp barrier
    exec_time += omp_get_wtime();

    std::cout << "seq: " << exec_time << std::endl;

    if(!std::equal(a_true.begin(), a_true.end(), a.begin(), a.end())) {
        std::cerr << "uncorrect work" << std::endl;
        std::abort();
    }
    std::cout << "OK!" << std::endl;
#endif

    return 0;
}