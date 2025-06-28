#ifndef TIMER_H
#define TIMER_H

#include "Types.h"

#include <boost/timer/timer.hpp>



namespace Slab {
    using CPUTimer = boost::timer::cpu_timer;
    using CPUTimes = boost::timer::cpu_times;
    using Nanosecond = boost::timer::nanosecond_type;

    class Timer {
    public:
        Timer() = default;

        ~Timer() {}

        void reset() { timer = CPUTimer(); }

        DevFloat getElTime_sec()   const { return (DevFloat)timer.elapsed().wall * 1e-9; }

        DevFloat getElTime_msec()  const { return (DevFloat)timer.elapsed().wall * 1e-6; }

        DevFloat getElTime_musec() const { return (DevFloat)timer.elapsed().wall * 1e-3; }

        boost::timer::nanosecond_type
        getElTime_nsec()  const { return timer.elapsed().wall; }

        void stop() { timer.stop(); }

        void resume() { timer.resume(); }

    private:
        CPUTimer timer = CPUTimer();
    };

}

#endif

