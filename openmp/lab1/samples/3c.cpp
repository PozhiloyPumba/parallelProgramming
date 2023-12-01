#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <omp.h>

#define CHECK_CORRECT
#include "common.hpp"


int main(int argc, char **argv) {
    std::vector<Type> a(ISIZE * JSIZE);
    std::vector<Type> b(ISIZE * JSIZE);

    for (int i = 0; i < ISIZE; ++i){
        for (int j = 0; j < JSIZE; ++j) {
            a[i * JSIZE + j] = 10 * i + j;
            b[i * JSIZE + j] = 0;
        }
    }

    #pragma omp barrier
    double exec_time = -omp_get_wtime();

    // D = {'=', '='}
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < ISIZE; ++i)
        for (int j = 0; j < JSIZE; ++j)
           a[i * JSIZE + j] = sin(0.002 * a[i * JSIZE + j]);

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < ISIZE - 4; ++i)
        for (int j = 1; j < JSIZE; ++j)
           b[i * JSIZE + j] = a[(i + 4) * JSIZE + j - 1] * 1.5;

    #pragma omp barrier
    exec_time += omp_get_wtime();

    std::cout << "parallel: " << exec_time << std::endl;

#ifdef CHECK_CORRECT
    std::vector<Type> a_true(ISIZE * JSIZE);
    std::vector<Type> b_true(ISIZE * JSIZE);
    
    for (int i = 0; i < ISIZE; ++i) {
        for (int j = 0; j < JSIZE; ++j) {
            a_true[i * JSIZE+ j] = 10 * i + j;
            b_true[i * JSIZE + j] = 0;
        }
    }

    #pragma omp barrier
    exec_time = -omp_get_wtime();

    for (int i = 0; i < ISIZE; ++i)
        for (int j = 0; j < JSIZE; ++j)
           a_true[i * JSIZE + j] = sin(0.002 * a_true[i * JSIZE + j]);

    for (int i = 0; i < ISIZE - 4; ++i)
        for (int j = 1; j < JSIZE; ++j)
           b_true[i * JSIZE + j] = a_true[(i + 4) * JSIZE + j - 1] * 1.5;

    #pragma omp barrier
    exec_time += omp_get_wtime();

    std::cout << "seq: " << exec_time << std::endl;

    if(!std::equal(b_true.begin(), b_true.end(), b.begin(), b.end())) {
        std::cerr << "uncorrect work" << std::endl;
        std::abort();
    }
    std::cout << "OK!" << std::endl;
#endif

    return 0;
}