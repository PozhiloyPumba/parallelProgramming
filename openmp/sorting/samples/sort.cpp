#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <random>

#define OPTIMIZED
#include <sort.hpp>

#define CHECK_CORRECT
// #define DUMP_SORTED
// #define INPUT

using TypeArr = int;

int main()
{
    int n;
    std::cin >> n;

    std::mt19937 rng;
#ifdef INPUT
    std::vector<TypeArr> vec(n);
    std::copy_n (std::istream_iterator<TypeArr>(std::cin), n, vec.begin());
#else
    auto randomNumberBetween = [](int low, int high)
    {
        auto randomFunc = [distribution_ = std::uniform_int_distribution<TypeArr>(low, high), 
                        random_engine_ = std::mt19937{ std::random_device{}() }]() mutable
        {
            return distribution_(random_engine_);
        };
        return randomFunc;
    };
    std::vector<TypeArr> vec;

    std::generate_n(std::back_inserter(vec), n, randomNumberBetween(-10000000, 10000000));
#endif

#ifdef CHECK_CORRECT
    std::vector<TypeArr> check(n);
    std::copy_n (vec.begin(), n, check.begin());
#endif

    double exec_time = -omp_get_wtime();

    Sort::sort(vec);

    exec_time += omp_get_wtime();

    std::cout << "my: " << exec_time << std::endl;

#ifdef CHECK_CORRECT
    exec_time = -omp_get_wtime();

    std::sort(check.begin(), check.end());
    exec_time += omp_get_wtime();

    std::cout << "std::sort: " << exec_time << " " << std::boolalpha << (check == vec) << std::endl;
#endif

#ifdef DUMP_SORTED
    std::copy (vec.begin (), vec.end (), std::ostream_iterator<TypeArr> (std::cout, " "));
    std::cout << std::endl;
#endif

    return 0;
}
