#ifndef TIMER_HPP__
#define TIMER_HPP__

#include <chrono>

namespace Support {

class Timer {
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::high_resolution_clock::time_point end_;

public:
    inline void timerInit () { start_ = std::chrono::high_resolution_clock::now (); }
    inline void timerEnd () { end_ = std::chrono::high_resolution_clock::now (); }
    inline long getTimeNs ()
    {
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::nanoseconds> (end_ - start_);
        return elapsed_seconds.count ();
    }
    inline long getTimeMs ()
    {
        return getTimeNs () / 1000000;
    }
};

}
#endif