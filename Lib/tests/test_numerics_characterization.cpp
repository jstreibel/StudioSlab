//
// Wave 0 characterization tests for numerics stacks.
//

#include <catch2/catch_all.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <numbers>
#include <thread>

#include "Utils/RandUtils.h"

#include "Math/Numerics/NumericTask.h"
#include "Math/Numerics/ODE/Steppers/RungeKutta4.h"
#include "Math/Numerics/ODE/Steppers/Euler.h"
#include "Math/Numerics/ODE/Output/Sockets/OutputSnapshots.h"
#include "Math/Numerics/ODE/Solver/LinearStepSolver.h"

#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Trigonometric.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/IntegerPowerFunctions.h"
#include "Math/Function/RtoR/Model/Operators/RtoRLaplacian.h"
#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREquationState.h"
#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBoundaryCondition.h"
#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRSolver.h"
#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREnergyCalculator.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/NullFunction.h"
#include "Models/Stochastic-Path-Integral/SPI-State.h"
#include "Models/Stochastic-Path-Integral/SPI-BC.h"
#include "Models/Stochastic-Path-Integral/SPI-Solver.h"

#include "Models/MolecularDynamics/MolecularDynamicsRecipe.h"
#include "Models/MolecularDynamics/Hamiltonians/MoleculesState.h"
#include "Models/MolecularDynamics/MolDynNumericConfig.h"

#include "Math/Numerics/Metropolis/RtoR/RtoR-Action-Metropolis-Recipe.h"
#include "Models/KleinGordon/RtoR-Montecarlo/RtoR-Hamiltonian-MetropolisHastings-Recipe.h"

namespace Slab {
    namespace {
        bool GTestStartupFlag = false;
    }

    void Startup() { GTestStartupFlag = true; }
    bool IsStarted() { return GTestStartupFlag; }
}

namespace {
    auto Mean(const Slab::RealArray &data) -> Slab::DevFloat {
        const auto n = data.size();
        if (n == 0) return 0.0;

        Slab::DevFloat sum = 0.0;
        for (size_t i = 0; i < n; ++i) sum += data[i];

        return sum / static_cast<Slab::DevFloat>(n);
    }

    auto Variance(const Slab::RealArray &data, Slab::DevFloat mean) -> Slab::DevFloat {
        const auto n = data.size();
        if (n == 0) return 0.0;

        Slab::DevFloat acc = 0.0;
        for (size_t i = 0; i < n; ++i) {
            const auto d = data[i] - mean;
            acc += d * d;
        }

        return acc / static_cast<Slab::DevFloat>(n);
    }

    auto WeightedChecksum(const Slab::RealArray &data) -> Slab::DevFloat {
        Slab::DevFloat checksum = 0.0;
        for (size_t i = 0; i < data.size(); ++i) {
            checksum += static_cast<Slab::DevFloat>(i + 1) * data[i];
        }
        return checksum;
    }

    auto MeanRadius(const Slab::Graphics::PointContainer &q) -> Slab::DevFloat {
        if (q.empty()) return 0.0;

        Slab::DevFloat sum = 0.0;
        for (const auto &point : q) {
            sum += std::sqrt(point.x * point.x + point.y * point.y);
        }
        return sum / static_cast<Slab::DevFloat>(q.size());
    }

    auto KineticEnergy(const Slab::Graphics::PointContainer &p) -> Slab::DevFloat {
        Slab::DevFloat sum = 0.0;
        for (const auto &mom : p) {
            sum += 0.5 * (mom.x * mom.x + mom.y * mom.y);
        }
        return sum;
    }

    auto StateChecksum(const Slab::Graphics::PointContainer &q,
                       const Slab::Graphics::PointContainer &p) -> Slab::DevFloat {
        const auto n = std::min(q.size(), p.size());
        Slab::DevFloat checksum = 0.0;

        for (size_t i = 0; i < n; ++i) {
            checksum += static_cast<Slab::DevFloat>(i + 1) * q[i].x;
            checksum += static_cast<Slab::DevFloat>(i + 3) * q[i].y;
            checksum += static_cast<Slab::DevFloat>(i + 5) * p[i].x;
            checksum += static_cast<Slab::DevFloat>(i + 7) * p[i].y;
        }

        return checksum;
    }

    auto SumAbs(const Slab::RealArray &data) -> Slab::DevFloat {
        Slab::DevFloat sum = 0.0;
        for (size_t i = 0; i < data.size(); ++i) {
            sum += std::abs(data[i]);
        }
        return sum;
    }

    class FScalarState final : public Slab::Math::Base::EquationState {
        Slab::DevFloat Value = 0.0;

        static auto Cast(const Slab::Math::Base::EquationState &state) -> const FScalarState & {
            return dynamic_cast<const FScalarState &>(state);
        }

    public:
        explicit FScalarState(Slab::DevFloat v = 0.0) : Value(v) {}

        auto GetValue() const -> Slab::DevFloat { return Value; }
        void SetValue(Slab::DevFloat v) { Value = v; }

        Slab::Math::Base::EquationState &
        StoreAddition(const Slab::Math::Base::EquationState &a, const Slab::Math::Base::EquationState &b) override {
            Value = Cast(a).Value + Cast(b).Value;
            return *this;
        }

        Slab::Math::Base::EquationState &
        StoreSubtraction(const Slab::Math::Base::EquationState &a, const Slab::Math::Base::EquationState &b) override {
            Value = Cast(a).Value - Cast(b).Value;
            return *this;
        }

        Slab::Math::Base::EquationState &Add(const Slab::Math::Base::EquationState &a) override {
            Value += Cast(a).Value;
            return *this;
        }

        Slab::Math::Base::EquationState &Subtract(const Slab::Math::Base::EquationState &a) override {
            Value -= Cast(a).Value;
            return *this;
        }

        Slab::Math::Base::EquationState &
        StoreScalarMultiplication(const Slab::Math::Base::EquationState &a, Slab::DevFloat k) override {
            Value = Cast(a).Value * k;
            return *this;
        }

        Slab::Math::Base::EquationState &Multiply(Slab::DevFloat k) override {
            Value *= k;
            return *this;
        }

        [[nodiscard]] auto category() const -> Slab::Str override {
            return "test|scalar";
        }

        [[nodiscard]] auto Replicate(std::optional<Slab::Str> Name) const
            -> Slab::TPointer<Slab::Math::Base::EquationState> override {
            (void) Name;
            return Slab::New<FScalarState>(0.0);
        }

        auto setData(const Slab::Math::Base::EquationState &other) -> void override {
            Value = Cast(other).Value;
        }
    };

    class FNoBoundaryCondition final : public Slab::Math::Base::BoundaryConditions {
    public:
        explicit FNoBoundaryCondition(const Slab::Math::Base::EquationState_constptr &prototype)
        : BoundaryConditions(prototype) {}

        void Apply(Slab::Math::Base::EquationState &toFunction, Slab::DevFloat t) const override {
            (void) toFunction;
            (void) t;
        }
    };

    class FProbeLinearStepSolver final : public Slab::Math::Base::LinearStepSolver {
        Slab::Vector<Slab::DevFloat> &SampledTimes;

    public:
        explicit FProbeLinearStepSolver(const Slab::TPointer<FNoBoundaryCondition> &bc,
                                        Slab::Vector<Slab::DevFloat> &times)
        : LinearStepSolver(bc)
        , SampledTimes(times) {}

        Slab::Math::Base::EquationState &
        F(const Slab::Math::Base::EquationState &in,
          Slab::Math::Base::EquationState &out,
          Slab::DevFloat t) override {
            SampledTimes.emplace_back(t);

            const auto &inState = dynamic_cast<const FScalarState &>(in);
            auto &outState = dynamic_cast<FScalarState &>(out);
            outState.SetValue(inState.GetValue());

            return out;
        }
    };

    class FFixedNumericConfig final : public Slab::Math::FNumericConfig {
        Slab::UIntBig NSteps = 0;

    public:
        explicit FFixedNumericConfig(Slab::UIntBig nSteps)
        : FNumericConfig(false)
        , NSteps(nSteps) {}

        [[nodiscard]] auto Get_n() const -> Slab::UIntBig override { return NSteps; }
        [[nodiscard]] auto to_string() const -> Slab::Str override { return "test|fixed-numeric-config"; }
    };

    class FCountingStepper final : public Slab::Math::FStepper {
        Slab::Vector<size_t> StepCalls;
        size_t TotalStepped = 0;
        Slab::Math::Base::EquationState_ptr State = Slab::New<FScalarState>(0.0);

    public:
        void Step(size_t n_steps) override {
            StepCalls.emplace_back(n_steps);
            TotalStepped += n_steps;

            auto &scalar = dynamic_cast<FScalarState &>(*State);
            scalar.SetValue(static_cast<Slab::DevFloat>(TotalStepped));
        }

        [[nodiscard]] auto GetCurrentState() const -> Slab::Math::Base::EquationState_constptr override {
            return State;
        }

        [[nodiscard]] auto GetStepCalls() const -> const Slab::Vector<size_t> & { return StepCalls; }
        [[nodiscard]] auto GetTotalStepped() const -> size_t { return TotalStepped; }
    };

    class FCountingRecipe final : public Slab::Math::Base::FNumericalRecipe {
        Slab::TPointer<FCountingStepper> CountingStepper;

    public:
        explicit FCountingRecipe(size_t nSteps)
        : FNumericalRecipe(Slab::New<FFixedNumericConfig>(nSteps),
                           "Wave2 Test Recipe",
                           "Wave2 test numerics recipe",
                           false) {}

        auto BuildOutputSockets() -> Slab::Math::Base::OutputSockets override { return {}; }

        auto BuildStepper() -> Slab::TPointer<Slab::Math::FStepper> override {
            CountingStepper = Slab::New<FCountingStepper>();
            return CountingStepper;
        }

        [[nodiscard]] auto GetCountingStepper() const -> Slab::TPointer<FCountingStepper> {
            return CountingStepper;
        }
    };

    class FRecordingOutputChannel final : public Slab::Math::FOutputChannel {
        Slab::Vector<size_t> OutputSteps;

    protected:
        auto HandleOutput(const Slab::Math::FOutputPacket &packet) -> void override {
            OutputSteps.emplace_back(packet.GetSteps());
        }

    public:
        explicit FRecordingOutputChannel(int intervalSteps)
        : FOutputChannel("Wave2 output recorder", intervalSteps, "Records output steps for tests") {}

        [[nodiscard]] auto GetOutputSteps() const -> const Slab::Vector<size_t> & { return OutputSteps; }
    };

    class FDummySnapshotOutput final : public Slab::Math::FOutputSnapshot {
    protected:
        void _outputToFile(std::ofstream &file) override {
            (void) file;
        }

    public:
        FDummySnapshotOutput()
        : FOutputSnapshot("wave3-dummy-snapshot", 4) {}
    };

    auto RunTaskAndWait(Slab::Math::FNumericTask &task) -> Slab::Core::ETaskStatus {
        std::thread worker([&task] { task.Start(); });

        while (true) {
            const auto status = task.GetStatus();
            if (status != Slab::Core::TaskNotInitialized && status != Slab::Core::TaskRunning) {
                task.Release();
                worker.join();
                return status;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

TEST_CASE("Wave0 characterization - KG RK4", "[Wave0][Numerics][KG][RK4]") {
    using namespace Slab;
    using namespace Slab::Math;
    using namespace Slab::Models::KGRtoR;

    RandUtils::SeedUniformReal(11);
    RandUtils::SeedUniformUInt(13);
    RandUtils::SeedGaussianNoise(17);

    constexpr UInt N = 128;
    constexpr DevFloat L = 2.0 * std::numbers::pi_v<DevFloat>;
    constexpr DevFloat xMin = -L * 0.5;
    constexpr DevFloat xMax = +L * 0.5;
    constexpr DevFloat dt = 0.005;
    constexpr CountType nSteps = 400;

    auto makeField = [&]() {
        return New<RtoR::NumericFunction_CPU>(
            N, xMin, xMax, RtoR::NumericFunction::Standard1D_PeriodicBorder);
    };

    auto prototype = New<FEquationState>(makeField(), makeField());

    auto initialPhi = New<RtoR::FSine>(1.0, 1.0);
    auto initialDPhi = New<RtoR::NullFunction>();
    auto bc = New<FBoundaryCondition>(prototype, initialPhi, initialDPhi);

    auto laplacian = New<RtoR::RtoRLaplacian>();
    auto potential = New<RtoR::FHarmonicPotential>(0.0);
    auto solver = New<KGRtoRSolver>(bc, laplacian, potential);

    auto stepper = New<FRungeKutta4>(solver, dt);

    const auto *initialState = dynamic_cast<const FEquationState *>(stepper->GetCurrentState().get());
    REQUIRE(initialState != nullptr);

    FKGEnergy energy(New<RtoR::FHarmonicPotential>(0.0));
    const auto &initialPhiRef = dynamic_cast<const RtoR::NumericFunction &>(initialState->getPhi());
    const auto &initialDPhiRef = dynamic_cast<const RtoR::NumericFunction &>(initialState->getDPhiDt());
    energy.computeEnergies(initialPhiRef, initialDPhiRef);
    const auto initialEnergy = energy.GetTotalEnergy();

    stepper->Step(nSteps);

    const auto *finalState = dynamic_cast<const FEquationState *>(stepper->GetCurrentState().get());
    REQUIRE(finalState != nullptr);

    const auto &phi = finalState->getPhi().getSpace().getHostData(true);
    const auto &dphi = finalState->getDPhiDt().getSpace().getHostData(true);

    const auto &finalPhiRef = dynamic_cast<const RtoR::NumericFunction &>(finalState->getPhi());
    const auto &finalDPhiRef = dynamic_cast<const RtoR::NumericFunction &>(finalState->getDPhiDt());
    energy.computeEnergies(finalPhiRef, finalDPhiRef);
    const auto finalEnergy = energy.GetTotalEnergy();

    const auto phiMean = Mean(phi);
    const auto dphiMean = Mean(dphi);
    const auto phiVar = Variance(phi, phiMean);
    const auto dphiVar = Variance(dphi, dphiMean);
    const auto phiChecksum = WeightedChecksum(phi);

    CAPTURE(initialEnergy, finalEnergy, phiMean, phiVar, dphiMean, dphiVar, phiChecksum);

    REQUIRE(std::isfinite(initialEnergy));
    REQUIRE(std::isfinite(finalEnergy));
    REQUIRE(std::isfinite(phiMean));
    REQUIRE(std::isfinite(phiVar));
    REQUIRE(std::isfinite(dphiMean));
    REQUIRE(std::isfinite(dphiVar));
    REQUIRE(std::isfinite(phiChecksum));

    CHECK(initialEnergy == Catch::Approx(1.55727).margin(1e-4));
    CHECK(finalEnergy == Catch::Approx(1.55806).margin(1e-4));
    CHECK(phiMean == Catch::Approx(0.0).margin(1e-12));
    CHECK(phiVar == Catch::Approx(0.080684).margin(1e-5));
    CHECK(dphiMean == Catch::Approx(0.0).margin(1e-10));
    CHECK(dphiVar == Catch::Approx(0.412707).margin(1e-5));
    CHECK(phiChecksum == Catch::Approx(-1047.28).margin(1e-2));
}

TEST_CASE("Wave0 characterization - SPI Euler", "[Wave0][Numerics][SPI][Euler]") {
    using namespace Slab;
    using namespace Slab::Math;
    using namespace Slab::Models::StochasticPathIntegrals;

    RandUtils::SeedUniformReal(19);
    RandUtils::SeedUniformUInt(23);
    RandUtils::SeedGaussianNoise(29);

    constexpr UInt N = 48;
    constexpr UInt M = 48;
    constexpr DevFloat L = 1.0;
    constexpr DevFloat xMin = -L * 0.5;
    constexpr DevFloat tMin = 0.0;
    constexpr DevFloat h = L / static_cast<DevFloat>(N);
    constexpr DevFloat dTau = 0.02;
    constexpr CountType nSteps = 12;

    auto phi = New<R2toR::NumericFunction_CPU>(N, M, xMin, tMin, h, h);
    phi->Set(R2toR::NullFunction());

    auto prototype = New<SPIState>(phi);
    auto bc = New<SPIBC>(prototype);
    auto solver = New<SPISolver>(bc);
    auto stepper = New<FEuler>(solver, dTau);

    stepper->Step(nSteps);

    const auto *state = dynamic_cast<const SPIState *>(stepper->GetCurrentState().get());
    REQUIRE(state != nullptr);

    const auto data = state->getPhi()->getSpace().getHostData(true);
    const auto mean = Mean(data);
    const auto var = Variance(data, mean);
    const auto checksum = WeightedChecksum(data);

    CAPTURE(mean, var, checksum);

    REQUIRE(std::isfinite(mean));
    REQUIRE(std::isfinite(var));
    REQUIRE(std::isfinite(checksum));

    CHECK(mean == Catch::Approx(1.43521e22).epsilon(1e-4));
    CHECK(var == Catch::Approx(2.60261e53).epsilon(1e-4));
    CHECK(checksum == Catch::Approx(2.53239e28).epsilon(1e-4));
}

TEST_CASE("Wave0 characterization - MolecularDynamics Verlet", "[Wave0][Numerics][MolDyn][Verlet]") {
    using namespace Slab;
    using namespace Slab::Models::MolecularDynamics;

    RandUtils::SeedUniformReal(31);
    RandUtils::SeedUniformUInt(37);
    RandUtils::SeedGaussianNoise(41);

    auto recipe = New<FMolecularDynamicsRecipe>();
    const auto config = DynamicPointerCast<MolDynNumericConfig>(recipe->GetNumericConfig());
    REQUIRE(config != nullptr);
    config->Set_t(50.0);

    auto stepper = recipe->BuildStepper();
    REQUIRE(stepper != nullptr);

    stepper->Step(40);

    const auto *state = dynamic_cast<const FMoleculesState *>(stepper->GetCurrentState().get());
    REQUIRE(state != nullptr);

    const auto &q = state->first();
    const auto &p = state->second();

    const auto meanRadius = MeanRadius(q);
    const auto kinetic = KineticEnergy(p);
    const auto checksum = StateChecksum(q, p);

    CAPTURE(meanRadius, kinetic, checksum);

    REQUIRE(std::isfinite(meanRadius));
    REQUIRE(std::isfinite(kinetic));
    REQUIRE(std::isfinite(checksum));

    CHECK(meanRadius == Catch::Approx(36.3873).margin(1e-3));
    CHECK(kinetic == Catch::Approx(0.012438).margin(1e-6));
    CHECK(checksum == Catch::Approx(3300.3).margin(1e-1));
}

TEST_CASE("Wave0 characterization - Metropolis RtoR", "[Wave0][Numerics][Metropolis][RtoR]") {
    using namespace Slab;
    using namespace Slab::Math;

    RandUtils::SeedUniformReal(43);
    RandUtils::SeedUniformUInt(47);
    RandUtils::SeedGaussianNoise(53);

    auto recipe = New<FRtoRActionMetropolisRecipe>(500);
    auto stepper = recipe->BuildStepper();
    REQUIRE(stepper != nullptr);

    stepper->Step(20);

    const auto field = recipe->getField();
    REQUIRE(field != nullptr);
    const auto data = field->getSpace().getHostData(true);

    const auto mean = Mean(data);
    const auto var = Variance(data, mean);
    const auto checksum = WeightedChecksum(data);

    CAPTURE(mean, var, checksum);

    REQUIRE(std::isfinite(mean));
    REQUIRE(std::isfinite(var));
    REQUIRE(std::isfinite(checksum));

    CHECK(mean == Catch::Approx(-7.1346e-06).margin(1e-8));
    CHECK(var == Catch::Approx(5.80436e-09).margin(1e-11));
    CHECK(checksum == Catch::Approx(-0.00696318).margin(1e-8));
}

TEST_CASE("Wave1 regression - RK4 uses stage times", "[Wave1][Numerics][RK4]") {
    using namespace Slab;
    using namespace Slab::Math;

    Vector<DevFloat> sampledTimes;

    auto prototype = New<FScalarState>(1.0);
    auto bc = New<FNoBoundaryCondition>(prototype);
    auto solver = New<FProbeLinearStepSolver>(bc, sampledTimes);
    auto stepper = New<FRungeKutta4>(solver, 0.1);

    stepper->Step(1);

    REQUIRE(sampledTimes.size() == 4);
    CHECK(sampledTimes[0] == Catch::Approx(0.0).margin(1e-12));
    CHECK(sampledTimes[1] == Catch::Approx(0.05).margin(1e-12));
    CHECK(sampledTimes[2] == Catch::Approx(0.05).margin(1e-12));
    CHECK(sampledTimes[3] == Catch::Approx(0.1).margin(1e-12));
}

TEST_CASE("Wave1 regression - Hamiltonian Metropolis updates pi", "[Wave1][Numerics][Metropolis][Hamiltonian]") {
    using namespace Slab;
    using namespace Slab::Models::KGRtoR::Metropolis;

    RandUtils::SeedUniformReal(59);
    RandUtils::SeedUniformUInt(61);
    RandUtils::SeedGaussianNoise(67);

    auto recipe = New<FRtoRHamiltonianMetropolisHastingsRecipe>(50);
    auto fields = recipe->getField();
    REQUIRE(fields.ϕ != nullptr);
    REQUIRE(fields.π != nullptr);

    const auto initialPiAbs = SumAbs(fields.π->getSpace().getHostData(true));
    CHECK(initialPiAbs == Catch::Approx(0.0).margin(1e-12));

    auto stepper = recipe->BuildStepper();
    REQUIRE(stepper != nullptr);

    stepper->Step(3);

    const auto phiAbs = SumAbs(fields.ϕ->getSpace().getHostData(true));
    const auto piAbs = SumAbs(fields.π->getSpace().getHostData(true));

    CAPTURE(phiAbs, piAbs);

    CHECK(phiAbs > 0.0);
    CHECK(piAbs > 0.0);
}

TEST_CASE("Wave2 regression - NumericTask batches long cycles", "[Wave2][Numerics][Task]") {
    using namespace Slab;

    constexpr size_t totalSteps = 1000000;
    auto recipe = New<FCountingRecipe>(totalSteps);
    auto outputRecorder = New<FRecordingOutputChannel>(1000003);

    auto outputManager = New<Math::FOutputManager>(totalSteps);
    outputManager->AddOutputChannel(outputRecorder);

    auto task = New<Math::FNumericTask>(recipe, false);
    task->SetOutputManager(outputManager);

    REQUIRE(RunTaskAndWait(*task) == Core::TaskSuccess);

    const auto stepper = recipe->GetCountingStepper();
    REQUIRE(stepper != nullptr);

    const auto &stepCalls = stepper->GetStepCalls();
    REQUIRE_FALSE(stepCalls.empty());

    size_t maxCycle = 0;
    size_t totalIntegrated = 0;
    for (const auto nCycles : stepCalls) {
        maxCycle = std::max(maxCycle, nCycles);
        totalIntegrated += nCycles;
    }

    CAPTURE(stepCalls.size(), maxCycle, totalIntegrated);

    CHECK(stepCalls.size() > 1);
    CHECK(maxCycle < totalSteps);
    CHECK(totalIntegrated == totalSteps);
    CHECK(stepper->GetTotalStepped() == totalSteps);
    CHECK(task->GetSteps() == totalSteps);

    const Vector<size_t> expectedOutputs = {0, totalSteps};
    CHECK(outputRecorder->GetOutputSteps() == expectedOutputs);
}

TEST_CASE("Wave2 regression - NumericTask preserves output cadence", "[Wave2][Numerics][Task]") {
    using namespace Slab;

    constexpr size_t totalSteps = 230;
    auto recipe = New<FCountingRecipe>(totalSteps);
    auto outputRecorder = New<FRecordingOutputChannel>(50);

    auto outputManager = New<Math::FOutputManager>(totalSteps);
    outputManager->AddOutputChannel(outputRecorder);

    auto task = New<Math::FNumericTask>(recipe, false);
    task->SetOutputManager(outputManager);

    REQUIRE(RunTaskAndWait(*task) == Core::TaskSuccess);

    const Vector<size_t> expectedOutputs = {0, 50, 100, 150, 200, totalSteps};
    CHECK(outputRecorder->GetOutputSteps() == expectedOutputs);
}

TEST_CASE("Wave3 regression - Snapshot output schedules future steps only", "[Wave3][Numerics][Output]") {
    using namespace Slab;

    auto snapshotOutput = New<FDummySnapshotOutput>();
    snapshotOutput->addSnapshotStep(10);
    snapshotOutput->addSnapshotStep(30);
    snapshotOutput->addSnapshotStep(20);

    CHECK(snapshotOutput->ComputeNextRecStep(0) == 10);
    CHECK(snapshotOutput->ComputeNextRecStep(10) == 20);
    CHECK(snapshotOutput->ComputeNextRecStep(20) == 30);
    CHECK(snapshotOutput->ComputeNextRecStep(30) > 30);
}

TEST_CASE("Wave3 regression - Snapshot output handles empty schedule", "[Wave3][Numerics][Output]") {
    using namespace Slab;

    auto snapshotOutput = New<FDummySnapshotOutput>();

    CHECK(snapshotOutput->ComputeNextRecStep(0) > 0);
    CHECK(snapshotOutput->ComputeNextRecStep(123) > 123);
}
