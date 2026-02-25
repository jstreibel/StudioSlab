#ifndef STUDIOSLAB_SIMULATION_EVENT_V2_H
#define STUDIOSLAB_SIMULATION_EVENT_V2_H

#include "Math/Numerics/ODE/Solver/EquationState.h"

#include <optional>

namespace Slab::Math::Numerics::V2 {

    class FSimulationSessionV2;

    enum class ERunModeV2 {
        FiniteSteps,
        FiniteSimulationTime,
        OpenEnded
    };

    enum class EEventReasonV2 {
        Initial,
        Scheduled,
        Forced,
        Final,
        AbortFinal
    };

    enum class ETriggerDomainV2 {
        Step,
        SimulationTime,
        WallClock
    };

    enum class EDeliveryModeV2 {
        Synchronous,
        LatestOnly,
        Buffered
    };

    struct FSimulationCursorV2 {
        UIntBig Step = 0;
        std::optional<DevFloat> SimulationTime = std::nullopt;
        double WallClockSeconds = 0.0;
    };

    struct FRunLimitsV2 {
        ERunModeV2 Mode = ERunModeV2::FiniteSteps;
        std::optional<UIntBig> MaxSteps = std::nullopt;
        std::optional<DevFloat> MaxSimulationTime = std::nullopt;
        std::optional<double> MaxWallClockSeconds = std::nullopt;
    };

    struct FSimulationEventV2 {
        FSimulationCursorV2 Cursor;
        EEventReasonV2 Reason = EEventReasonV2::Scheduled;
        Base::EquationState_constptr State = nullptr;
        TVolatile<const FSimulationSessionV2> SessionRef;
        UIntBig PublishedVersion = 0;
        bool bIsRealtimeBestEffort = false;
    };

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_SIMULATION_EVENT_V2_H
