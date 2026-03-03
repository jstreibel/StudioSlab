#include "RtoR-Hamiltonian-MetropolisHastings-RecipeV2.h"

#include "Core/SlabCore.h"
#include "Utils/RandUtils.h"

#include "Math/Numerics/Metropolis/Metropolis-Setup.h"
#include "Math/Numerics/Metropolis/MetropolisAlgorithm.h"
#include "Math/Numerics/Metropolis/MontecarloStepper.h"
#include "Math/Numerics/V2/Listeners/ConsoleProgressListenerV2.h"
#include "Math/Numerics/V2/Listeners/SessionLiveViewPublisherListenerV2.h"
#include "Math/Numerics/V2/Runtime/StepperSessionV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"

#include "Metropolis-FieldStateV2.h"

#include <algorithm>
#include <cmath>

namespace Slab::Models::KGRtoR::Metropolis::V2 {

    using namespace Slab::Math;
    using namespace Slab::Math::Numerics::V2;

    namespace {
        using FRandomSite = Int;
        using FNewValue = Pair<DevFloat, DevFloat>;
        using FMetropolisAlgo = FMetropolisAlgorithm<FRandomSite, FNewValue>;
        using FMetropolisSetupT = FMetropolisSetup<FRandomSite, FNewValue>;
    } // namespace

    FRtoRHamiltonianMetropolisHastingsRecipeV2::FRtoRHamiltonianMetropolisHastingsRecipeV2(
            const UIntBig maxSteps,
            const UIntBig consoleIntervalSteps,
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            const bool bInRunEndless)
    : MaxSteps(maxSteps)
    , ConsoleIntervalSteps(std::max<UIntBig>(1, consoleIntervalSteps))
    , LiveView(liveView)
    , bRunEndless(bInRunEndless) {
    }

    auto FRtoRHamiltonianMetropolisHastingsRecipeV2::BuildStepper() -> TPointer<FStepper> {
        class FMetropolisStepperV2 final : public Math::FStepper {
            TPointer<FMetropolisAlgo> Algorithm;
            TPointer<FMetropolisFieldStateV2> State;

        public:
            FMetropolisStepperV2(TPointer<FMetropolisAlgo> algorithm,
                                 TPointer<FMetropolisFieldStateV2> state)
            : Algorithm(std::move(algorithm))
            , State(std::move(state)) {
            }

            auto Step(const size_t nSteps) -> void override {
                if (Algorithm == nullptr) return;
                for (size_t step = 0; step < nSteps; ++step) {
                    Algorithm->step();
                }
            }

            [[nodiscard]] auto GetCurrentState() const -> Math::Base::EquationState_constptr override {
                return State;
            }
        };

        constexpr DevFloat xMin = -0.5;
        constexpr DevFloat xMax = 0.5;
        constexpr UInt N = 10000;

        PhiField = New<RtoR::NumericFunction_CPU>(N, xMin, xMax);
        PiField = New<RtoR::NumericFunction_CPU>(N, xMin, xMax);

        const auto phiField = PhiField;
        const auto piField = PiField;

        FMetropolisSetupT setup;

        constexpr Temperature T = 1e-2;
        constexpr DevFloat dPhiMax = 1e0;
        constexpr DevFloat dPiMax = 1e-2;

        setup.should_accept = [=](const DevFloat dE) {
            const auto boltzmann = std::exp(-dE / T);
            return RandUtils::RandomUniformReal01() < std::min<DevFloat>(1.0, boltzmann);
        };

        setup.draw_value = [=](const FRandomSite site) {
            const auto &phiSpace = phiField->getSpace();
            const auto &piSpace = piField->getSpace();

            const auto phiOld = phiSpace.getHostData()[site];
            const auto piOld = piSpace.getHostData()[site];

            return FNewValue{
                RandUtils::RandomUniformReal(phiOld - dPhiMax, phiOld + dPhiMax),
                RandUtils::RandomUniformReal(piOld - dPiMax, piOld + dPiMax)
            };
        };

        setup.ΔS = [=](const FRandomSite site, const FNewValue newVal) {
            const auto &phiData = phiField->getSpace().getHostData();
            const auto &piData = piField->getSpace().getHostData();
            const auto fieldSize = static_cast<Int>(phiField->N);

            auto PhiAt = [&](const Int idx) {
                const auto wrapped = idx < 0 ? fieldSize + idx :
                        idx > fieldSize - 1 ? idx - fieldSize : idx;
                return phiData[wrapped];
            };

            const auto phiOld = PhiAt(site);
            const auto phiNew = newVal.first;

            const auto piOld = piData[site];
            const auto piNew = newVal.second;

            const auto dX = phiField->getSpace().getMetaData().geth(0);

            const auto dK = static_cast<DevFloat>(0.5) * (piOld * piOld - piNew * piNew);
            const auto dV = std::fabs(phiOld) - std::fabs(phiNew);
            const auto dW = (static_cast<DevFloat>(0.25) / (dX * dX)) *
                    ((phiNew - phiOld) * (PhiAt(site - 2) + PhiAt(site + 2)) + phiOld * phiOld - phiNew * phiNew);

            return dX * (dK + dW + dV);
        };

        setup.sample_locations = [=]() {
            constexpr auto borderSize = 0;

            Vector<FRandomSite> samples(phiField->N);
            for (auto &sample : samples) {
                sample = borderSize + static_cast<FRandomSite>(
                        RandUtils::RandomUniformUInt() % (phiField->N - 2 * borderSize));
            }

            return samples;
        };

        setup.modify = [=](const FRandomSite site, const FNewValue value) {
            phiField->getSpace().getHostData()[site] = value.first;
            piField->getSpace().getHostData()[site] = value.second;
        };

        const auto metropolis = New<FMetropolisAlgo>(setup);
        auto state = New<FMetropolisFieldStateV2>(phiField, piField);
        return New<FMetropolisStepperV2>(metropolis, state);
    }

    auto FRtoRHamiltonianMetropolisHastingsRecipeV2::BuildSession() -> TPointer<FSimulationSessionV2> {
        return New<FStepperSessionV2>(BuildStepper());
    }

    auto FRtoRHamiltonianMetropolisHastingsRecipeV2::BuildDefaultSubscriptions() -> Vector<FSubscriptionV2> {
        const auto liveViewInterval = LiveViewIntervalSteps > 0 ? LiveViewIntervalSteps : ConsoleIntervalSteps;
        auto console = New<FConsoleProgressListenerV2>(
            bRunEndless ? std::optional<UIntBig>{} : std::optional<UIntBig>{MaxSteps},
            "Metropolis RtoR Console V2");

        Vector<FSubscriptionV2> subscriptions = {{
            New<FEveryNStepsTriggerV2>(ConsoleIntervalSteps),
            console,
            EDeliveryModeV2::Synchronous,
            true,
            true
        }};

        if (LiveView != nullptr) {
            auto publisher = New<FSessionLiveViewPublisherListenerV2>(LiveView, "Metropolis Live View Publisher V2");
            subscriptions.push_back({
                New<FEveryNStepsTriggerV2>(liveViewInterval),
                publisher,
                EDeliveryModeV2::Synchronous,
                true,
                true
            });
        }

        return subscriptions;
    }

    auto FRtoRHamiltonianMetropolisHastingsRecipeV2::SetLiveViewIntervalSteps(const UIntBig liveViewIntervalSteps) -> void {
        LiveViewIntervalSteps = liveViewIntervalSteps == 0
            ? UIntBig(0)
            : std::max<UIntBig>(1, liveViewIntervalSteps);
    }

    auto FRtoRHamiltonianMetropolisHastingsRecipeV2::GetRunLimits() const -> FRunLimitsV2 {
        FRunLimitsV2 limits;
        if (bRunEndless) {
            limits.Mode = ERunModeV2::OpenEnded;
            limits.MaxSteps = std::nullopt;
            return limits;
        }
        limits.Mode = ERunModeV2::FiniteSteps;
        limits.MaxSteps = MaxSteps;
        return limits;
    }

    auto FRtoRHamiltonianMetropolisHastingsRecipeV2::GetPhiField() const -> TPointer<RtoR::NumericFunction_CPU> {
        return PhiField;
    }

    auto FRtoRHamiltonianMetropolisHastingsRecipeV2::GetPiField() const -> TPointer<RtoR::NumericFunction_CPU> {
        return PiField;
    }

    auto FRtoRHamiltonianMetropolisHastingsRecipeV2::GetLiveView() const -> TPointer<Math::LiveData::V2::FSessionLiveViewV2> {
        return LiveView;
    }

} // namespace Slab::Models::KGRtoR::Metropolis::V2
