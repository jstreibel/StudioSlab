#ifndef TIMER_H
#define TIMER_H

#include "Types.h"

#include <boost/timer/timer.hpp>



namespace Slab {
    using CPUTimer = boost::timer::cpu_timer;
    using CPUTimes = boost::timer::cpu_times;
    using Nanosecond = boost::timer::nanosecond_type;

    class FTimer {
    public:
        FTimer() = default;

        ~FTimer() = default;

        void reset() { timer = CPUTimer(); }

        [[nodiscard]] DevFloat GetElapsedTime_Seconds()   const { return (DevFloat)timer.elapsed().wall * 1e-9; }

        [[nodiscard]] DevFloat getElTime_msec()  const { return (DevFloat)timer.elapsed().wall * 1e-6; }

        [[nodiscard]] DevFloat getElTime_musec() const { return (DevFloat)timer.elapsed().wall * 1e-3; }

        [[nodiscard]] boost::timer::nanosecond_type
        getElTime_nsec()  const { return timer.elapsed().wall; }

        void stop() { timer.stop(); }

        void resume() { timer.resume(); }

    private:
        CPUTimer timer = CPUTimer();
    };

}

#endif

