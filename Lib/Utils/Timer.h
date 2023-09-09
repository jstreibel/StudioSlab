#ifndef TIMER_H
#define TIMER_H

#include "Types.h"

#include <boost/timer/timer.hpp>

typedef boost::timer::cpu_timer CPUTimer;


class Timer
{
public:
    Timer() = default;
    ~Timer() {}

    void reset() { timer = CPUTimer(); }
    Real getElTime_sec() const { return timer.elapsed().wall*1e-9; }
    Real getElTime_msec() const { return timer.elapsed().wall*1e-6; }
    Real getElTime_musec() const { return timer.elapsed().wall*1e-3; }
    Real getElTime_nsec() const { return timer.elapsed().wall; }

    void stop() { timer.stop(); }
    void resume() { timer.resume(); }

private:
    CPUTimer timer = CPUTimer();
};

#endif
