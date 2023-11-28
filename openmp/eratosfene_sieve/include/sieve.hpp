#ifndef SIEVE_HPP_
#define SIEVE_HPP_

#include <algorithm>
#include <iostream>
#include <vector>

// #include "App.hpp"

namespace Sieve {

class Eratosfene final {
private:
    std::vector<char> sieve_; // char because paralleling cannot use every cell atomically
    size_t true_size_;
    bool filled_ = false;

    inline int getIndex(int i) const {
        return (i - 2) / 6 * 2 + int(i % 6 < 3);
    }

public:
    Eratosfene (const size_t n = 0)  // ctor
        : sieve_(n / 3 + 3, true), true_size_(n)
    {
    }

    void fill();

    // return true if prime
    bool check(int candidate) const;

    int getQuantityOfPrimes(int upperBound) const;
};

}  // namespace Sieve

#endif
