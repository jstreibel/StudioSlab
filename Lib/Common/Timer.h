#ifndef TIMER_H
#define TIMER_H

#include "STDLibInclude.h"

#include <boost/timer/timer.hpp>

typedef boost::timer::cpu_timer CPUTimer;


class Timer
{
public:
    Timer() = default;
    ~Timer() {}

    void reset() { timer = CPUTimer(); }
    double getElTime_sec() const { return timer.elapsed().wall*1e-9; }
    double getElTime_msec() const { return timer.elapsed().wall*1e-6; }
    double getElTime_musec() const { return timer.elapsed().wall*1e-3; }
    double getElTime_nsec() const { return timer.elapsed().wall; }

private:
    CPUTimer timer = CPUTimer();
};

#endif
