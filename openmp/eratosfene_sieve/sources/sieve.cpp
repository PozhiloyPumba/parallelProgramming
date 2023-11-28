#include "sieve.hpp"
#include <numeric>
#include <cmath>

#if defined(TBB)
#include <execution>
#define PAR std::execution::par,
#else
#define PAR
#endif

#if defined(PARALLEL_TASKS) || defined(PARALLEL_FOR_BASE)
#include <omp.h> 
#endif

namespace Sieve {

void Eratosfene::fill() {
    filled_ = true;

    long long limit_checker = std::sqrt(true_size_);

    #if defined(PARALLEL_TASKS)
    std::cout << "tasks" << std::endl;
    #pragma omp parallel
    {
        #pragma omp single
        {
            int step = 4;
            for(long long i = 5; i <= limit_checker; i += step) {
                step = 2 << (step == 2); // flipping between 2 and 4
                if (!sieve_[getIndex(i)])
                    continue;

                #pragma omp task shared(sieve_) firstprivate(i)
                {
                    for (long long j = i * i; j < (long long)(true_size_); j += 2 * i) {
                        if (std::abs(j % 6 - 3) != 2)
                            continue;

                        int index = getIndex(j);
                        sieve_[index] = false;
                    }
                }
            }
        }
    }
    #elif defined(PARALLEL_FOR_BASE)
    std::cout << "for" << std::endl;
    int step = 4;
    for(long long i = 5; i <= limit_checker; i += step) {
        step = 2 << (step == 2); // flipping between 2 and 4
        if (!sieve_[getIndex(i)])
            continue;

        long long div_size = (true_size_ / i - i) / 2 + 1;
        long long init_j = i * i;
        long long j_step = 2 * i;
        #pragma omp parallel for
        for (int k = 0; k < div_size; ++k) {
            long long j = init_j + j_step * k;
            if (std::abs(j % 6 - 3) != 2)
                continue;

            int index = getIndex(j);
            sieve_[index] = false;
        }
    }
    #else
    std::cout << "seq" << std::endl;
    int step = 4;
    for(long long i = 5; i <= limit_checker; i += step) {
        step = 2 << (step == 2); // flipping between 2 and 4
        if (!sieve_[getIndex(i)])
            continue;

        for (long long j = i * i; j < (long long)(true_size_); j += 2 * i) {
            if (std::abs(j % 6 - 3) != 2)
                continue;

            sieve_[getIndex(j)] = false;
        }
    }
    #endif
}

// return true if prime
bool Eratosfene::check(int candidate) const {
    if (!filled_) {
        std::cerr << "fill sieve" << std::endl;
        return false;
    }
    if ((candidate == 2) || (candidate == 3))
        return true;
    if (!(candidate % 2) || !(candidate % 3))
        return false;

    return sieve_[getIndex(candidate)];
}

int Eratosfene::getQuantityOfPrimes(int upperBound) const {
    if (!filled_) {
        std::cerr << "fill sieve" << std::endl;
        return false;
    }
    if (upperBound > (long long)(true_size_)) {
        std::cerr << "sieve size less then check size" << std::endl;
        return 0;
    }

    if (upperBound <= 2)
        return 0;
    if (upperBound == 3)
        return 1;

    // switch (upperBound % 6) {
    //     case 0: border = upperBound + 1; break;
    //     case 1:
    //     case 5: border = upperBound; break;
    //     case 2:
    //     case 3:
    //     case 4: border = upperBound / 6 * 6 + 5; break;
    //     default:;
    // }

    // this trick for except switch which in comment
    int border = upperBound + int(upperBound % 6 == 0) + (std::abs(upperBound % 6 - 3) <= 1) * (5 - upperBound % 6);

    return std::reduce(PAR sieve_.begin(), sieve_.begin() + getIndex(border), 0) + 2;
}

}