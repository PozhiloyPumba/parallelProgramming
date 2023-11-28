#include <sieve.hpp>
#include <timer.hpp>

int main () {
    int n;
    std::cin >> n;
    Sieve::Eratosfene s(n);

    Support::Timer t;
    t.timerInit ();

    s.fill();

    t.timerEnd ();

    // for (int i = 2; i < n; ++i) {
    //     if (s.check(i)) {
    //         std::cout << i << std::endl;
    //     }
    // }
#if defined (TEST)
    std::cout << "made this" << std::endl;
#else
    std::cout << "time: " << t.getTimeMs() << std::endl;
    std::cout << "Prime count: " << s.getQuantityOfPrimes(n) << std::endl;
#endif

    return 0;
}