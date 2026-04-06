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

        void Reset() { timer = CPUTimer(); }

        [[nodiscard]] DevFloat GetElapsedTimeSeconds() const { return (DevFloat)timer.elapsed().wall * 1e-9; }

        [[nodiscard]] DevFloat GetElapsedTimeMsec() const { return (DevFloat)timer.elapsed().wall * 1e-6; }

        [[nodiscard]] DevFloat GetElapsedTimeUsec() const { return (DevFloat)timer.elapsed().wall * 1e-3; }

        [[nodiscard]] boost::timer::nanosecond_type GetElapsedTimeNsec() const { return timer.elapsed().wall; }

        void Stop() { timer.stop(); }

        void Resume() { timer.resume(); }

        [[deprecated("Use Reset")]]
        void reset() { Reset(); }

        [[deprecated("Use GetElapsedTimeSeconds")]]
        [[nodiscard]] DevFloat GetElapsedTime_Seconds() const { return GetElapsedTimeSeconds(); }

        [[deprecated("Use GetElapsedTimeMsec")]]
        [[nodiscard]] DevFloat getElTime_msec() const { return GetElapsedTimeMsec(); }

        [[deprecated("Use GetElapsedTimeUsec")]]
        [[nodiscard]] DevFloat getElTime_musec() const { return GetElapsedTimeUsec(); }

        [[deprecated("Use GetElapsedTimeNsec")]]
        [[nodiscard]] boost::timer::nanosecond_type getElTime_nsec() const { return GetElapsedTimeNsec(); }

        [[deprecated("Use Stop")]]
        void stop() { Stop(); }

        [[deprecated("Use Resume")]]
        void resume() { Resume(); }

    private:
        CPUTimer timer = CPUTimer();
    };

}

#endif
