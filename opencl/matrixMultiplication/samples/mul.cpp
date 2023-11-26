#include <matrix.hpp>

using TypeMatrix = float;

int main () {
    int m, n;
    std::cin >> m >> n;
    matrix::Matrix<TypeMatrix> m1(m, n);
    std::cin >> m1;

    std::cin >> m >> n;
    matrix::Matrix<TypeMatrix> m2(m, n);
    std::cin >> m2;

    OpenCLApp::Timer t;
    t.timerInit ();
    matrix::Matrix<TypeMatrix> res = m1*m2;
    t.timerEnd ();

#if defined (TEST)
    std::cout << res << std::endl;
#else
    std::cout << "time: " << t.getTimeMs() << std::endl;
#endif

    return 0;
}