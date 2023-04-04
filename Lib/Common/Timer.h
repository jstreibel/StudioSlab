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
    double getElTimeMSec() { return timer.elapsed().wall*1e-6; }

private:
    CPUTimer timer = CPUTimer();
};

#endif
