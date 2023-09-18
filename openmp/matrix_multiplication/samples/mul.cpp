#include <omp.h>
#include <matrix.hpp>

using TypeMatrix = double;

int main () {
    int m, n;
    std::cin >> m >> n;
    matrix::Matrix<TypeMatrix> m1(m, n);
    std::cin >> m1;

    std::cin >> m >> n;
    matrix::Matrix<TypeMatrix> m2(m, n);
    std::cin >> m2;
    
    double itime, ftime, exec_time;
    itime = omp_get_wtime();

    matrix::Matrix<TypeMatrix> res = m1*m2;
    
    ftime = omp_get_wtime();

    exec_time = ftime - itime;

    std::cout << exec_time << std::endl;
    // std::cout << res << std::endl;
    return 0;
}