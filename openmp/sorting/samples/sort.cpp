#include <sort.hpp>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>

#define CHECK_CORRECT

using TypeArr = int;

int main () {
    int n;
    std::cin >> n;
    std::vector<TypeArr> vec(n);

    std::copy_n (std::istream_iterator<TypeArr>(std::cin), n, vec.begin());

#ifdef CHECK_CORRECT
    std::vector<TypeArr> check(n);
    std::copy_n (vec.begin(), n, check.begin());
#endif

    double exec_time = -omp_get_wtime();

    sort(vec);

    exec_time += omp_get_wtime();

    std::cout << exec_time << std::endl;

#ifdef CHECK_CORRECT
    std::sort(check.begin(), check.end());
    std::cout << (check == vec) << std::endl;
#endif

    // std::copy (vec.begin (), vec.end (), std::ostream_iterator<TypeArr> (std::cout, " "));
    // std::cout << std::endl;

    return 0;
}
