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
            a[i * JSIZE + j] = 10 * i + j;

    #pragma omp barrier
    double exec_time = -omp_get_wtime();

    // D = {'=', '='}
    #pragma omp parallel for collapse(2)
    // D_i = 0
    for (int i = 0; i < ISIZE; ++i)
        // D_j = 0
        for (int j = 0; j < JSIZE; ++j)
           a[i * JSIZE + j] = sin(2 * a[i * JSIZE + j]);

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

    for (int i = 0; i < ISIZE; ++i)
        for (int j = 0; j < JSIZE; ++j)
           a_true[i * JSIZE + j] = sin(2 * a_true[i * JSIZE + j]);

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